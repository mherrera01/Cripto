'''
print("Which file should be converted to capital letters only?")
filename = input()
print("How should be the new file named?")
newFilename = input()
'''

filename = "el_quijote.txt"
newFilename = "test2.txt"

allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

f = open(filename, "r")
fileContent = f.read()

fileContent = fileContent.upper()

for c in ''.join(set(fileContent)):
    if c not in allowedChars:
        print(c)
        fileContent = fileContent.replace(str(c), "")

f.close()

f = open(newFilename, "w")

f.write(fileContent)

f.close()