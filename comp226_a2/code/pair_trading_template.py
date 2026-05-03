import backtrader as bt
import pandas as pd
import numpy as np
import yfinance as yf
from statsmodels.regression.linear_model import OLS
from statsmodels.tools.tools import add_constant


def extra1(prices1, prices2):
    """
    Compute hedge ratio using linear regression
    :param prices1: stock 1 prices with length `lookback`
    :param prices2: stock 2 prices with length `lookback`
    :return: LS_const, hedge_ratio
    """
    # Write your code here.

    return


def extra2(price1, price2, hedge_ratio, constant):
    """
    Compute the spread.
    :param price1: stock 1 price
    :param price2: stock 2 price
    :param hedge_ratio: self-explained.
    :param constant: self-explained.
    :return: Spread
    """
    # Write your code here.

    return



def extra3(spread):
    """
    Compute Z-score.
    :param spread: a list of spreads
    :return: Z-score
    """
    # Write your code here.

    return


def extra4():
    """
    :return: the final value after running your cerebro.
    """
    # Write your code here.

    return



# Define the PairTradingStrategy
class PairTradingStrategy(bt.Strategy):
    params = dict(
        zscore_entry=2.0,  # Z-score for entering trades
        zscore_exit=0.1,  # Z-score for exiting trades
        lookback=30  # Rolling window for spread calculation
    )

    def __init__(self):
        # Keep references to the two data feeds
        self.stock1 = self.datas[0].close
        self.stock2 = self.datas[1].close

        # Rolling window data
        self.spread = []
        self.hedge_ratio = None
        self.LS_const = None

    def next(self):
        # Ensure enough data is available for calculations
        if len(self) < self.params.lookback:
            return

        # Compute hedge ratio using linear regression
        self.LS_const, self.hedge_ratio = extra1(prices1=self.stock1.get(size=self.params.lookback),
                                                 prices2=self.stock2.get(size=self.params.lookback))

        # Calculate the spread
        spread_value = extra2(self.stock1[0], self.stock2[0], self.hedge_ratio, self.LS_const)
        self.spread.append(spread_value)

        if len(self.spread) < self.params.lookback:
            return

        # Ensure we have enough spread data for z-score calculation
        if len(self.spread) > self.params.lookback:
            self.spread.pop(0)

        # Calculate z-score
        zscore = extra3(self.spread)

        # Entry and exit conditions
        # Write your code here.



# Function to download stock data using yfinance
def download_data(stock1_ticker, stock2_ticker, start_date, end_date):
    # Write your code here.

    return


# Main function to set up the backtesting.
def main():
    # Write your code here for backtesting.





    # Run the backtest
    print("Starting Portfolio Value: %.2f" % cerebro.broker.getvalue())
    cerebro.run()
    print("Final Portfolio Value: %.2f" % cerebro.broker.getvalue())




if __name__ == "__main__":
    main()
