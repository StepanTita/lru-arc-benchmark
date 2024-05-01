from random import choice

import faker


def main():
    fak = faker.Faker()
    ips = [fak.ipv4() for i in range(1000)]

    with open('data/ips_uniform.log', 'w') as f:
        for _ in range(100_000):
            f.write(f'{choice(ips)}\n')


if __name__ == '__main__':
    main()
