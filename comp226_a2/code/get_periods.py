import hashlib
import argparse
from datetime import datetime, timedelta

def username_to_unique_date(usernames: list, start_date: str = "2014-01-01", end_date: str = "2022-12-31") -> dict:
    """
    Maps usernames to unique dates within the specified range.

    Parameters:
        usernames (list): A list of unique username strings.
        start_date (str): The start date in "YYYY-MM-DD" format. Default is "2016-01-01".
        end_date (str): The end date in "YYYY-MM-DD" format. Default is "2022-01-01".

    Returns:
        dict: A dictionary mapping usernames to unique date strings.
    """
    # Parse the start and end dates
    start = datetime.strptime(start_date, "%Y-%m-%d")
    end = datetime.strptime(end_date, "%Y-%m-%d")

    # Calculate the total number of days in the range
    total_days = (end - start).days

    # Set to track used dates
    used_dates = set()
    username_date_map = {}

    for username in usernames:
        # Hash the username to get a deterministic integer
        hashed_value = int(hashlib.sha256(username.encode()).hexdigest(), 16)

        # Try mapping the hash to a unique date
        for offset in range(total_days):
            candidate_date = start + timedelta(days=(hashed_value + offset) % total_days)
            if candidate_date not in used_dates:
                used_dates.add(candidate_date)
                username_date_map[username] = candidate_date.strftime("%Y-%m-%d")
                break

    return username_date_map


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Get Period", add_help=False)
    parser.add_argument("user_name", type=str, help="Your Liverpool user name")
    args = parser.parse_args()

    usernames = [args.user_name]
    unique_dates = username_to_unique_date(usernames)

    print("The start of your trading date is", unique_dates[args.user_name])
    print("The end of your trading date is", "2024-12-31")
