import os
import struct


#open the name file and read in the data

name_file = open('c:/tmp/allnames.txt')
file_names = name_file.readlines()
name_file.close()
file_names = []

for i in file_names:
	#get rid of trailing line feed
	if i[-1] == '\012':
		one_name = i[0:-1]
	else
		one_name = i

	#open the model

	if one_name not in file_names:
		file_names.append(one_name)

name_file=open('c:/tmp/tnames.txt','w')

for i in file_names:
	name_file.write(i)
	name_file.write('\n')

name_file.close()