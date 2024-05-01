from collections import defaultdict

def main():
    with open('data/ips_uniform.log', 'r') as f:
        lines = f.readlines()

        ips = defaultdict(lambda: 0)
        for line in lines:
            ip = line.split(' ')[0]
            ips[ip] += 1

    print(ips)


if __name__ == '__main__':
    main()