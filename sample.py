import random
import csv

def export_matrix(box, output_file='matrix.csv'):
    with open(output_file, 'w') as f:
        writer = csv.writer(f, delimiter=',')
        for line in box:
            writer.writerow(line)


def random_matrix(rows, cols, max_number=10000):
    return [[random.randint(0, max_number) for j in range(rows)] for i in range(cols)]

if __name__ == '__main__':
    box = random_matrix(32, 32)
    export_matrix(box)
