ALPHABET_LENGTH = 26

def tonum(letter: str) -> int:
    assert(len(letter) == 1)
    return ord(letter) - ord('a')


def toletter(num: int) -> str:
    assert(num >= 0 and num < ALPHABET_LENGTH)
    return chr(num + ord('a'))


class CaesarCipher:
    def __init__(self, key: int = 3) -> None:
        self.key = key

    def encrypt(self, plain: str) -> str:
        plain = plain.lower()
        cipher = ""

        for l in plain:
            if l.isalpha():
                cipher += toletter((tonum(l) + self.key) % ALPHABET_LENGTH)
            else:
                cipher += l

        return cipher

    def decrypt(self, cipher: str) -> str:
        plain = ""

        for l in cipher:
            if l.isalpha():
                plain += toletter((tonum(l) - self.key) % ALPHABET_LENGTH)
            else:
                plain += l

        return plain
    

def crack(cipher: str) -> list[str]:
     tries = []
     for k in range(1, ALPHABET_LENGTH+1):
         pc = CaesarCipher(key=k)
         tries.append(pc.decrypt(cipher))
     return tries
