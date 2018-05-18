import random

def generate_input(filepath):
    with open(filepath, 'w') as out_file:
        for i in xrange(0, 1024 * 1024):
            out_file.write(str(random.uniform(0.0, 100.0)))
            out_file.write('\n')

for i in xrange(1, 5):
    generate_input('input_example_{0}.txt'.format(i))
