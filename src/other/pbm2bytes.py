
pbm = open('penrose.pbm', 'rb')

byteArr = pbm.read()[9:]

print(len(byteArr))

out = open('pen.txt', 'w')
out.write('{')

for p in range(5, -1, -1):
    for c in range(0, 56):
        out.write('%d, ' % byteArr[p + (c * 6)])

pbm.close()
out.close()
