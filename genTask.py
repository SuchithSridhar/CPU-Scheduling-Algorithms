import sys
from random import randint


class Task:
    def __init__(self, name: str, arrival: int, burst: int):
        self.name = name
        self.arrival = arrival
        self.burst = burst

    def __str__(self):
        return f"{self.name},{self.arrival},{self.burst}"


def genTasks(count: int) -> list[Task]:
    cur_arrival = 0
    tasks: list[Task] = []
    arrival_buffer = randint(20, 40)
    burst_range = randint(20, 50)

    for i in range(1, count+1):
        arrival = randint(cur_arrival, cur_arrival + arrival_buffer)
        tasks.append(Task(
            f"T{i}",
            arrival,
            randint(1, burst_range)
        ))
        cur_arrival = arrival + 1

    return tasks


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Please provide number of tasks to generate")
        exit(1)

    count = int(sys.argv[1])
    tasks: list[Task] = genTasks(count)
    for task in tasks:
        print(task)
