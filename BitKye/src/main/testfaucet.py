from src.main.hook import token, walletaddress
import requests

def startFaucet():
    params = (('token', token),)
    data = '{"address": "{0}", "amount": 100000}'.format(walletaddress)

    response = requests.post('https://api.blockcypher.com/v1/bcy/test/faucet', params=params, data=data)
    if response:
        print(response)
    else:
        print("nope")


startFaucet   