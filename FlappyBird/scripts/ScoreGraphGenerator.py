import matplotlib.pyplot as plt
import pandas as pd

file = open("../data/ML_ScoreProgression.txt", "r")
data = {"Generation": [], "Score": []}

gen = 1
for line in file:
    data["Score"].append(float(line.strip()))
    data["Generation"].append(gen)
    gen += 1

file.close()

df = pd.DataFrame(data)
plt.scatter(df["Generation"], df["Score"])
plt.xlabel("Generation")
plt.ylabel("Score")
plt.title("Score over Time")
plt.show()