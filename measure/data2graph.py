import matplotlib.pyplot as plt
import os

dir_list = os.listdir("./measure/scenarios")
for folder in dir_list:
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

        bins = 10000

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
