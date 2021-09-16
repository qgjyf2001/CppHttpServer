import requests
import json
param={'data':'1','b':'2'}
url='http://127.0.0.1:8080/test'

files = {'file1':open('./main.cpp','rb'),'file2':open('./test.py','rb')}
r=requests.post(url,files=files).text
#r=requests.post(url,data=param).text
print(r)
