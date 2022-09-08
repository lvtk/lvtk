import argparse
import sys
import os

def encode (data, var_name='var'):
    out = []
    out.append('static unsigned char {var_name}[] = {{'.format(var_name=var_name))
    l = [ data[i:i+12] for i in range(0, len(data), 12) ]
    for i, x in enumerate(l):
        line = ', '.join([ '0x{val:02x}'.format(val=ord(c)) for c in x ])
        out.append('  {line}{end_comma}'.format(line=line, end_comma=',' if i<len(l)-1 else ''))
    out.append('};')
    out.append('static unsigned int {var_name}_len = {data_len};'.format(var_name=var_name, data_len=len(data)))
    return '\n'.join(out)

def main():
    parser = argparse.ArgumentParser(description='Generate binary header output')
    parser.add_argument('-i', '--input', required=True , help='Input file')
    parser.add_argument('-o', '--out', required=True , help='Output file')
    parser.add_argument('-v', '--var', required=False , help='Variable name to use in file')

    args = parser.parse_args()
    if not args:
        return 1

    with open(args.input, 'r') as f:
        data = f.read()

    var = os.path.basename (args.input).replace ('.', '_')

    out = encode(data, var)
    with open(args.out, 'w') as f:
        f.write(out)

    return 0

if __name__ == '__main__':
    sys.exit(main())
