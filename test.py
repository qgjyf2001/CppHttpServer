import requests
import json
param={'data':'1','b':'2'}
url='http://127.0.0.1:8080/test'
headers={'Content-Type':'application/json'}
r=requests.post(url,headers=headers,data=json.dumps(param)).text
#r=requests.post(url,data=param).text
print(r)
