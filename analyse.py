import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
from scipy.linalg import dft
from scipy.signal import find_peaks
from scipy.fft import fft

samplerate, data = wavfile.read('assets/C0.wav')

print(len(data))
print(samplerate)

winsize = 1024
wins = [data[x:x+winsize] for x in range(0, len(data), winsize)]

plt.figure()
plt.plot(data)
plt.show()

for idx,win in enumerate(wins):
    plt.figure()
    print(idx, list(win))
    d = abs(fft(win))
    d = d[:len(d) // 2]
    # d = list(map(lambda x: x if x > 0.5 * 10 ** 6 else 0, d))
    plt.plot(d)
    plt.show()

# notes = []
# for win in wins:
#     win = [x for x in win]
#     # plt.figure()
#     res = abs(fft(win))
#     res = res[0:(len(res) // 2)]
#     # plt.plot(res)
#     peaks = find_peaks(res, height=1.2 * 10 ** 5)
#     peak = None
#     peakval = 0
#     for i in range(len(peaks[0])):
#         idx = peaks[0][i]
#         val = peaks[1]['peak_heights'][i]
#         if val > peakval:
#             peak = idx
#             peakval = val
#     print(peak)
#     peak = peak if peak else 0
#     if peak < 1500:
#         print(f'Found freq peak at {peak * samplerate / winsize}')
#         notes += [peak * samplerate / winsize for _ in range(1024)]
#     else:
#         notes += [0 for _ in range(1024)]
# plt.show()

# plt.figure()
# plt.plot(data)
# plt.plot([note * 10 for note in notes])
# plt.show()
# 39

# data = []
# with open("dft", "r") as f:
#     lines = f.readlines()
#     for line in lines:
#         data.append(float(line))
# plt.figure()
# plt.plot(data)
# plt.show()
