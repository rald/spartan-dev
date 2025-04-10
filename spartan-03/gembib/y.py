f=open("adb.inf")

for i in f:
  print(i.split("|",1)[0])