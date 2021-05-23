import random

deck = list(range(52))
counts = {}
for card in deck:
    counts[card] = 0

cards_drawn = 0
while True:
    random.shuffle(deck)
    i = 0
    while i < len(deck):
        counts[deck[i]] += 1
        cards_drawn += 1
        if deck[i] == 16:
            break
        i += 1
    if cards_drawn > 1_000_000:
        break

for k, v in counts.items():
    print(f'{k}\t{v}')