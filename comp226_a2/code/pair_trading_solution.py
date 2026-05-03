import backtrader as bt
import pandas as pd
import numpy as np
import yfinance as yf
from statsmodels.regression.linear_model import OLS
from statsmodels.tools.tools import add_constant


# ──────────────────────────────────────────────────────────────────────────────
# EXTRA 1 – Hedge ratio via OLS regression
# ──────────────────────────────────────────────────────────────────────────────
def extra1(prices1, prices2):
    """
    Compute the hedge ratio (and constant) using OLS:
        prices1 = LS_const + hedge_ratio * prices2 + ε

    :param prices1: array-like of stock-1 prices (length = lookback)
    :param prices2: array-like of stock-2 prices (length = lookback)
    :return: (LS_const, hedge_ratio)
    """
    prices2_with_const = add_constant(np.array(prices2))
    model   = OLS(np.array(prices1), prices2_with_const)
    result  = model.fit()

    LS_const    = result.params[0]   # intercept
    hedge_ratio = result.params[1]   # slope

    return LS_const, hedge_ratio


# ──────────────────────────────────────────────────────────────────────────────
# EXTRA 2 – Spread
# ──────────────────────────────────────────────────────────────────────────────
def extra2(price1, price2, hedge_ratio, constant):
    """
    Compute the spread:
        spread = price1 - hedge_ratio * price2 + constant

    :param price1:      current price of stock 1
    :param price2:      current price of stock 2
    :param hedge_ratio: from Extra 1
    :param constant:    LS_const from Extra 1
    :return: spread value
    """
    spread = price1 - hedge_ratio * price2 + constant
    return spread


# ──────────────────────────────────────────────────────────────────────────────
# EXTRA 3 – Z-score
# ──────────────────────────────────────────────────────────────────────────────
def extra3(spread):
    """
    Compute the Z-score of the most-recent spread observation:
        z = (spread[-1] - mean(spread)) / std(spread)

    :param spread: list of spread values (length = lookback)
    :return: Z-score (float)
    """
    spread_arr = np.array(spread)
    mean       = np.mean(spread_arr)
    std        = np.std(spread_arr)
    if std == 0:
        return 0.0
    return float((spread_arr[-1] - mean) / std)


# ──────────────────────────────────────────────────────────────────────────────
# EXTRA 4 – Run the full pair-trading backtest and return the final value
# ──────────────────────────────────────────────────────────────────────────────
def extra4():
    """
    Build and run the pair-trading cerebro for PEP / KO.

    ── HOW TO USE ──────────────────────────────────────────────────────────────
    1. Run:  python get_periods.py <YOUR_USERNAME>
    2. Paste your start date into `start_date` below (end date = 2024-12-31).
    3. Replace `username` with your MWS username.
    ────────────────────────────────────────────────────────────────────────────

    :return: final portfolio value (float)
    """
    # ── TODO: fill in your username and start date ────────────────────────────
    start_date = "YOUR_START_DATE"   # e.g. "2017-06-14"
    end_date   = "2024-12-31"

    feed1, feed2 = download_data("PEP", "KO", start_date, end_date)

    cerebro = bt.Cerebro()
    cerebro.adddata(feed1, name="PEP")
    cerebro.adddata(feed2, name="KO")
    cerebro.addstrategy(PairTradingStrategy)
    cerebro.broker.setcash(1_000_000.0)
    cerebro.broker.setcommission(commission=0.001)

    cerebro.run()

    return cerebro.broker.getvalue()


# ──────────────────────────────────────────────────────────────────────────────
# Pair Trading Strategy
# ──────────────────────────────────────────────────────────────────────────────
class PairTradingStrategy(bt.Strategy):
    params = dict(
        zscore_entry=2.0,   # threshold to open a trade
        zscore_exit=0.1,    # threshold to close a trade
        lookback=30,        # rolling window for spread / hedge-ratio
    )

    def __init__(self):
        # References to the two close-price series
        self.stock1 = self.datas[0].close
        self.stock2 = self.datas[1].close

        # State
        self.spread      = []
        self.hedge_ratio = None
        self.LS_const    = None

    def next(self):
        # Need at least `lookback` bars
        if len(self) < self.params.lookback:
            return

        # ── Step 1: update hedge ratio via OLS ──────────────────────────────
        self.LS_const, self.hedge_ratio = extra1(
            prices1=self.stock1.get(size=self.params.lookback),
            prices2=self.stock2.get(size=self.params.lookback),
        )

        # ── Step 2: compute spread ───────────────────────────────────────────
        spread_value = extra2(self.stock1[0], self.stock2[0],
                               self.hedge_ratio, self.LS_const)
        self.spread.append(spread_value)

        # Keep window at most `lookback` long
        if len(self.spread) < self.params.lookback:
            return
        if len(self.spread) > self.params.lookback:
            self.spread.pop(0)

        # ── Step 3: compute Z-score ──────────────────────────────────────────
        zscore = extra3(self.spread)

        # ── Step 4: entry / exit logic ───────────────────────────────────────
        pos1 = self.getposition(self.datas[0]).size
        pos2 = self.getposition(self.datas[1]).size
        in_position = (pos1 != 0 or pos2 != 0)

        if not in_position:
            # Size of stock 2 = hedge_ratio × 100
            size2 = abs(self.hedge_ratio) * 100

            if zscore > self.params.zscore_entry:
                # Spread is too high: short PEP, long KO
                self.sell(data=self.datas[0], size=100)
                self.buy( data=self.datas[1], size=size2)

            elif zscore < -self.params.zscore_entry:
                # Spread is too low: long PEP, short KO
                self.buy( data=self.datas[0], size=100)
                self.sell(data=self.datas[1], size=size2)

        else:
            # Close both positions when Z-score reverts to zero
            if abs(zscore) < self.params.zscore_exit:
                self.close(data=self.datas[0])
                self.close(data=self.datas[1])


# ──────────────────────────────────────────────────────────────────────────────
# Helper – download and convert to BackTrader feeds
# ──────────────────────────────────────────────────────────────────────────────
def download_data(stock1_ticker, stock2_ticker, start_date, end_date):
    """
    Download adjusted-close data for two tickers and return
    (bt.feeds.PandasData, bt.feeds.PandasData).
    """
    feeds = []
    for ticker in (stock1_ticker, stock2_ticker):
        raw = yf.download(ticker, start=start_date, end=end_date,
                          auto_adjust=False, progress=False)

        # Flatten MultiIndex columns (newer yfinance)
        if isinstance(raw.columns, pd.MultiIndex):
            raw.columns = [col[0] for col in raw.columns]

        # Map Adj Close → Close (with fallback)
        if 'Adj Close' in raw.columns:
            raw['Close'] = raw['Adj Close']

        feed = bt.feeds.PandasData(
            dataname=raw,
            open='Open',
            high='High',
            low='Low',
            close='Close',
            volume='Volume',
            openinterest=-1,
        )
        feeds.append(feed)

    return feeds[0], feeds[1]


# ──────────────────────────────────────────────────────────────────────────────
# Main – wire everything together
# ──────────────────────────────────────────────────────────────────────────────
def main():
    """
    ── TODO: fill in your username and start date ────────────────────────────
    Run:  python get_periods.py <YOUR_USERNAME>
    Then paste the printed start date below; end date is always 2024-12-31.
    ────────────────────────────────────────────────────────────────────────────
    """
    start_date = "YOUR_START_DATE"   # e.g. "2017-06-14"
    end_date   = "2024-12-31"

    feed1, feed2 = download_data("PEP", "KO", start_date, end_date)

    cerebro = bt.Cerebro()
    cerebro.adddata(feed1, name="PEP")
    cerebro.adddata(feed2, name="KO")
    cerebro.addstrategy(PairTradingStrategy)
    cerebro.broker.setcash(1_000_000.0)
    cerebro.broker.setcommission(commission=0.001)

    print("Starting Portfolio Value: %.2f" % cerebro.broker.getvalue())
    cerebro.run()
    print("Final Portfolio Value: %.2f" % cerebro.broker.getvalue())


if __name__ == "__main__":
    main()
