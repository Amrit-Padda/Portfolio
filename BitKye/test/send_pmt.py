from bit import wif_to_key
from src.main.dbmanager import address
import time

wifs = ['cUGd57XqmDj2u3yqJqcR3sTz5ZCWemVV4hE8S9PuNtY3JLpRwHiU', 
        'cSjgP5ekYjtMvaDiE9MFd7Bm4xsRAy3sAoLjnfKF4D3Ahoqyd7jx',
        'cRULogm6wBcrNqoHDzFnrAGM2KtoNsAGkZudfe5BDwqrkx6isDLE',
        'cTDgRsqkyYtW4cPxyBS4Ft8KPfJ3x2GTxxLyfxFDrGqifg1wgKuy',
        'cRX5S2vgdHrFcbVNxeNEvVdorzfRQfiUSeZVLse5EYJkWVh8zoTX',
        'cQQzhPQyEiuRGTSGqLQMz9iSP1G3BkFDn3KFNa4M5pjXD4nobAQf',
        'cPh1Cg8m2oixZcpz8F7Ay4dD3tXfztQVNkZkTWSoMU9RYZLm1thU',
        'cTx6JKP3PyFNEyHturMNnAdLbXXidNxza3GunP3AAjpevU15ck5N',
        'cS1yWciUaraTWGaveDf4jgwrJec9pD34EUzHBD6BL7jkGcCwRWo2',
        'cVLpBLgWqoJN55eHvaWnGC2jshRPESHHWm9dbfXRKf99dd6ZGSvw']

keys = []
for line in wifs:
   keys.append(wif_to_key(line)) 

for x in range(1,10):
    keys[x].create_transaction([address, 0.0005, 'BTC'])
    time.sleep(5)

keys[0].create_transaction([address, 0.001, 'BTC'])