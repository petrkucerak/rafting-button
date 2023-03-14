import matplotlib.pyplot as plt
import os

dir_list = os.listdir("./measure/scenarios")
for folder in dir_list:
    plt.figure(figsize=(12, 8))
    folder_path = "./measure/scenarios/" + folder
    print(folder_path)
    sub_dir_list = os.listdir(folder_path)
    for file in sub_dir_list:
        print("Read file: ", file)
        path = folder_path + "/" + file
        f = open(path)

        label = file.replace("-data.txt", "") + " scenario"

        x = f.readline()
        y = f.readline()

        f.close()

        x = x.replace("time=[", "")
        x = x.replace(",]\n", "")
        y = y.replace("value=[", "")
        y = y.replace(",]", "")

        x = list(map(int, x.split(",")))
        y = list(map(int, y.split(",")))

        # simple filter
        # for i in range(0, len(x) - 7):
        #     index = x[i]
        #     if (x[i + 1] == index + 1):
        #         y[i] += (y[i + 1])/2

        #     if (x[i + 2] == index + 2):
        #         y[i] += (y[i + 2])/4

        #     if (x[i + 3] == index + 3):
        #         y[i] += (y[i + 3])/6

        #     if (x[i + 4] == index + 4):
        #         y[i] += (y[i + 4])/8

        #     if (x[i + 5] == index + 5):
        #         y[i] += (y[i + 5])/8

        #     if (x[i + 6] == index + 6):
        #         y[i] += (y[i + 6])/8

        #     if (x[i + 7] == index + 7):
        #         y[i] += (y[i + 7])/8
        # bins = 10000

        # plt.hist(y, bins=bins, density=True, histtype='step', cumulative=-1, label = label)
        plt.plot(x, y, label=label)

    plt.xlabel("Time [μs]")
    plt.ylabel("Number of messages")
    plt.legend()
    plt.grid(True)
    plt.yscale('log')
    plt.xscale('linear')
    plt.title("Round-trip time with ESP NOW in different scenarios")

    plt.show()
