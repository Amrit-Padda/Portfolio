from bit import PrivateKeyTestnet


wallets = []

for x in range(10):
    wallets.append(PrivateKeyTestnet())
    x = x + 1

print(wallets)
f = open(r"C:\Users\Amrit\Documents\GitHub\BitKye\test\wallets.txt", "a")

for wallet in wallets:
    f.write(wallet.to_wif() + " " + wallet.address + "\n")

f.close