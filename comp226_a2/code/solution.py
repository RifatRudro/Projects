import yfinance as yf
import pandas as pd
import numpy as np
import backtrader as bt


# ──────────────────────────────────────────────────────────────────────────────
# TASK 1 – Download data & create BackTrader feed
# ──────────────────────────────────────────────────────────────────────────────
def task_1(ticker_symbol, start_date, end_date):
    raw = yf.download(ticker_symbol, start=start_date, end=end_date,
                      auto_adjust=False, progress=False)

    # Flatten MultiIndex columns produced by newer yfinance versions
    if isinstance(raw.columns, pd.MultiIndex):
        raw.columns = [col[0] for col in raw.columns]

    # Ensure all required columns exist
    for col in ['Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume']:
        if col not in raw.columns:
            # Fallback or initialization if missing
            raw[col] = 0.0

    # Use Adj Close as Close (with fallback)
    if 'Adj Close' in raw.columns:
        raw['Close'] = raw['Adj Close']

    # Re-order to exactly match the required ['Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume'] order
    raw = raw[['Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume']]

    feed = bt.feeds.PandasData(
        dataname=raw,
        open='Open',
        high='High',
        low='Low',
        close='Close',
        volume='Volume',
        openinterest=-1,
    )
    return feed

# ──────────────────────────────────────────────────────────────────────────────
# TASK 2 – Create and configure a Cerebro
# ──────────────────────────────────────────────────────────────────────────────
def task_2(data, cash, commission, slippage_percentage):
    cerebro = bt.Cerebro()
    cerebro.adddata(data)
    cerebro.broker.setcash(cash)
    cerebro.broker.setcommission(commission=commission)
    cerebro.broker.set_slippage_perc(slippage_percentage)
    return cerebro


# ──────────────────────────────────────────────────────────────────────────────
# TASK 3 – Triple Moving Average (TMA) Strategy
# ──────────────────────────────────────────────────────────────────────────────
def task_3():
    """
    Implement the TMA strategy.
    Entry:  short > medium > long  → BUY
            short < medium < long  → SELL (short)
    Exit:   trailing stop-loss with trailpercent = 0.02 for both directions.
    """
    class TripleMovingAverageStrategy(bt.Strategy):
        params = (
            ('short_period',  10),
            ('medium_period', 50),
            ('long_period',  100),
        )

        def __init__(self):
            self.sma_short  = bt.indicators.SimpleMovingAverage(
                self.data.close, period=self.params.short_period)
            self.sma_medium = bt.indicators.SimpleMovingAverage(
                self.data.close, period=self.params.medium_period)
            self.sma_long   = bt.indicators.SimpleMovingAverage(
                self.data.close, period=self.params.long_period)

            # Bookkeeping required by notify_order / notify_trade
            self.order       = None
            self.buyprice    = None
            self.buycomm     = None
            self.bar_executed = None

        def next(self):
            # Don't stack orders – wait until the current one settles
            if self.order:
                return

            if not self.position:
                # ── Entry logic ──────────────────────────────────────────────
                if (self.sma_short[0] > self.sma_medium[0] and
                        self.sma_medium[0] > self.sma_long[0]):
                    # Bullish alignment → go long
                    self.order = self.buy()

                elif (self.sma_short[0] < self.sma_medium[0] and
                        self.sma_medium[0] < self.sma_long[0]):
                    # Bearish alignment → go short
                    self.order = self.sell()

            else:
                # ── Exit: trailing stop-loss ──────────────────────────────────
                if self.position.size > 0:          # long position
                    self.order = self.sell(
                        exectype=bt.Order.StopTrail, trailpercent=0.02)
                elif self.position.size < 0:        # short position
                    self.order = self.buy(
                        exectype=bt.Order.StopTrail, trailpercent=0.02)

        # ── Boilerplate logging (provided; do not change) ─────────────────────
        def log(self, txt, dt=None):
            dt = dt or self.datas[0].datetime.date(0)
            print('%s, %s' % (dt.isoformat(), txt))

        def notify_order(self, order):
            if order.status in [order.Submitted, order.Accepted]:
                return
            if order.status in [order.Completed]:
                if order.isbuy():
                    self.log(
                        'BUY EXECUTED, Price: %.2f, Cost: %.2f, Comm %.2f' %
                        (order.executed.price,
                         order.executed.value,
                         order.executed.comm))
                    self.buyprice = order.executed.price
                    self.buycomm  = order.executed.comm
                else:
                    self.log(
                        'SELL EXECUTED, Price: %.2f, Cost: %.2f, Comm %.2f' %
                        (order.executed.price,
                         order.executed.value,
                         order.executed.comm))
                self.bar_executed = len(self)
            elif order.status in [order.Canceled, order.Margin, order.Rejected]:
                self.log('Order Canceled/Margin/Rejected')
            self.order = None

        def notify_trade(self, trade):
            if not trade.isclosed:
                return
            self.log('OPERATION PROFIT, GROSS %.2f, NET %.2f' %
                     (trade.pnl, trade.pnlcomm))

        def stop(self):
            self.log(
                '(MA Periods are %2d, %2d, %2d) Ending Value %.2f' %
                (self.params.short_period,
                 self.params.medium_period,
                 self.params.long_period,
                 self.broker.getvalue()))

    return TripleMovingAverageStrategy


# ──────────────────────────────────────────────────────────────────────────────
# TASK 4 – Bollinger Bands Overbought / Oversold Strategy
# ──────────────────────────────────────────────────────────────────────────────
def task_4():
    """
    Implement the BB Overbought/Oversold strategy.
    Entry:  price < lower band → BUY  (oversold)
            price > upper band → SELL short (overbought)
    Exit:   price > upper band → close long
            price < lower band → close short
    """
    class BollingerBandsStrategy(bt.Strategy):
        params = (
            ('period',    20),   # lookback window
            ('devfactor',  3),   # standard-deviation multiplier
        )

        def __init__(self):
            self.bb = bt.indicators.BollingerBands(
                self.data.close,
                period=self.params.period,
                devfactor=self.params.devfactor,
            )

            # Bookkeeping required by notify_order / notify_trade
            self.order        = None
            self.buyprice     = None
            self.buycomm      = None
            self.bar_executed = None

        def next(self):
            if self.order:
                return

            price = self.data.close[0]
            upper = self.bb.lines.top[0]
            lower = self.bb.lines.bot[0]

            if not self.position:
                # ── Entry logic ──────────────────────────────────────────────
                if price < lower:
                    # Oversold → go long
                    self.order = self.buy()
                elif price > upper:
                    # Overbought → go short
                    self.order = self.sell()

            else:
                # ── Exit logic ───────────────────────────────────────────────
                if self.position.size > 0 and price > upper:
                    # Price crossed above upper band → close long
                    self.order = self.sell()
                elif self.position.size < 0 and price < lower:
                    # Price crossed below lower band → close short
                    self.order = self.buy()

        # ── Boilerplate logging (provided; do not change) ─────────────────────
        def log(self, txt, dt=None):
            dt = dt or self.datas[0].datetime.date(0)
            print('%s, %s' % (dt.isoformat(), txt))

        def notify_order(self, order):
            if order.status in [order.Submitted, order.Accepted]:
                return
            if order.status in [order.Completed]:
                if order.isbuy():
                    self.log(
                        'BUY EXECUTED, Price: %.2f, Cost: %.2f, Comm %.2f' %
                        (order.executed.price,
                         order.executed.value,
                         order.executed.comm))
                    self.buyprice = order.executed.price
                    self.buycomm  = order.executed.comm
                else:
                    self.log(
                        'SELL EXECUTED, Price: %.2f, Cost: %.2f, Comm %.2f' %
                        (order.executed.price,
                         order.executed.value,
                         order.executed.comm))
                self.bar_executed = len(self)
            elif order.status in [order.Canceled, order.Margin, order.Rejected]:
                self.log('Order Canceled/Margin/Rejected')
            self.order = None

        def notify_trade(self, trade):
            if not trade.isclosed:
                return
            self.log('OPERATION PROFIT, GROSS %.2f, NET %.2f' %
                     (trade.pnl, trade.pnlcomm))

    return BollingerBandsStrategy


# ──────────────────────────────────────────────────────────────────────────────
# TASK 5 – Add strategy + FixedSize sizer to Cerebro
# ──────────────────────────────────────────────────────────────────────────────
def task_5(cerebro, strategy_class, stake):
    """
    Add a strategy class to cerebro and configure a FixedSize sizer.
    """
    cerebro.addstrategy(strategy_class)
    cerebro.addsizer(bt.sizers.FixedSize, stake=stake)
    return cerebro


# ──────────────────────────────────────────────────────────────────────────────
# TASK 6 – Run Cerebro and return final portfolio value
# ──────────────────────────────────────────────────────────────────────────────
def task_6(cerebro):
    """
    Run the backtest and return the final portfolio value.
    """
    cerebro.run()
    return cerebro.broker.getvalue()


# ──────────────────────────────────────────────────────────────────────────────
# TASK 7 – Grid-search optimisation of TMA strategy
# ──────────────────────────────────────────────────────────────────────────────
def task_7(cerebro, stake):
    """
    Optimise the TMA strategy over a 3x3x3 grid of (short, medium, long) periods.

    ── HOW TO USE ──────────────────────────────────────────────────────────────
    1. Run:  python get_periods.py <YOUR_USERNAME>
       This gives you a start date (end date is always 2024-12-31).
    2. In main.py set those dates before calling this function.
    3. Run the optimisation once; read the printed "Ending Value" lines and the
       return value, then hard-code best_result below if you prefer.
    ────────────────────────────────────────────────────────────────────────────
    """
    # ── TODO: replace with your MWS username (part before @) ─────────────────
    username = "Rudro"

    TripleMovingAverageStrategy = task_3()

    cerebro.optstrategy(
        TripleMovingAverageStrategy,
        short_period=[5, 10, 20],
        medium_period=[50, 100, 150],
        long_period=[200, 250, 300],
    )
    cerebro.addsizer(bt.sizers.FixedSize, stake=stake)

    results = cerebro.run(maxcpu=1)

    # Find the parameter combination that yields the highest portfolio value
    best_value  = -float('inf')
    best_params = None

    for run in results:
        strat = run[0]
        val   = strat.broker.getvalue()
        if val > best_value:
            best_value  = val
            best_params = strat.params

    best_result = {
        "short_period":  best_params.short_period,
        "medium_period": best_params.medium_period,
        "long_period":   best_params.long_period,
        "value":         best_value,
    }

    return username, best_result


# ──────────────────────────────────────────────────────────────────────────────
# TASK 8 – Total compound return & Sharpe ratio of BB strategy
# ──────────────────────────────────────────────────────────────────────────────
def task_8():
    """
    Compute the total compound return and Sharpe ratio (risk-free = 0.02)
    for the Bollinger Band strategy over the student-specific trading period.

    ── HOW TO USE ──────────────────────────────────────────────────────────────
    1. Run:  python get_periods.py <YOUR_USERNAME>
    2. Paste your start date into `start_date` below (end date = 2024-12-31).
    3. Replace `username` with your MWS username.
    ────────────────────────────────────────────────────────────────────────────
    """
    # ── TODO: fill in your username and start date ────────────────────────────
    username   = "Rudro";
    start_date = "2019-06-08"
    end_date   = "2024-12-31"

    # Build cerebro with the required settings
    data    = task_1(ticker_symbol="AAPL",
                     start_date=start_date,
                     end_date=end_date)
    cerebro = task_2(data=data,
                     cash=1e6,
                     commission=0.001,
                     slippage_percentage=0.01)

    BollingerBandsStrategy = task_4()
    cerebro.addstrategy(BollingerBandsStrategy)
    cerebro.addsizer(bt.sizers.FixedSize, stake=10)

    # Attach analysers
    cerebro.addanalyzer(bt.analyzers.Returns,     _name='returns')
    cerebro.addanalyzer(bt.analyzers.SharpeRatio, _name='sharpe',
                        riskfreerate=0.02)

    results  = cerebro.run()
    strategy = results[0]

    total_compound_return = strategy.analyzers.returns.get_analysis()['rtot']
    sharpe_ratio = strategy.analyzers.sharpe.get_analysis()['sharperatio']

    returns = {
        "total_compound_return": total_compound_return,
        "sharpe_ratio":          sharpe_ratio,
    }

    return username, returns
