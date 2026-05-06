import numpy as np
from solution import *

########################################
## DO NOT CHANGE THE FOLLOWING FUNCTION.
########################################

def add_strategy_and_run(cerebro, strategy, stake):
    cerobro = task_5(cerebro=cerebro, strategy_class=strategy, stake=stake)
    final_value = task_6(cerebro=cerebro)

    return cerobro, final_value


def optimize_strategy_and_run(cerebro, stake):
    cerebro = task_7(cerebro=cerebro, stake=stake)

    return cerebro

def main(optimization=True, stake=10):
    # Load data

    ticker_symbol = "TSLA"
    start_date="2019-06-08"  # your date from get_periods.py
    end_date = "2024-12-31"

    data = task_1(ticker_symbol=ticker_symbol,
                  start_date=start_date,
                  end_date=end_date)

    # Create two cerebros
    cerebro_TMA = task_2(data=data,
                         cash=1e6,
                         commission=0.001,
                         slippage_percentage=0.01)
    cerebro_BB = task_2(data=data,
                        cash=1e6,
                        commission=0.001,
                        slippage_percentage=0.01)

    # Create strategies.
    if optimization:
        print("In the optimization mode:")
        cerebro_TMA = optimize_strategy_and_run(cerebro=cerebro_TMA, stake=stake)
    else:
        print("In the non-optimization mode:")
        Triple_Moving_Average_Strategy = task_3()
        Bollinger_Bands_Overbought_Oversold_Strategy = task_4()

        cerebro_TMA, final_value_TMA = add_strategy_and_run(cerebro=cerebro_TMA,
                                                            strategy=Triple_Moving_Average_Strategy,
                                                            stake=stake)

        cerebro_BB, final_value_BB = add_strategy_and_run(cerebro=cerebro_BB,
                                                          strategy=Bollinger_Bands_Overbought_Oversold_Strategy,
                                                          stake=stake)


        print("The final value for TMA strategy is {}".format(final_value_TMA))
        print("The final value for BB strategy is {}".format(final_value_BB))



if __name__ == "__main__":
    main(optimization=False, stake=10)
    # main(optimization=True, stake=10)


