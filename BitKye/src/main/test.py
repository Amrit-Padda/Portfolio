from bit import wif_to_key
priv = '' #removed for privacy
wallet = wif_to_key(priv)

def test():
    print(wallet.get_unspents(), wallet.get_balance())

test()