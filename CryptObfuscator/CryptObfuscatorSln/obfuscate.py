import sys
import os

with open(sys.argv[1], 'rb') as f:
	c = f.read()

start_mark = b"\xbb\xc3\xe1\x1c\x05\xca\xde\xc0\xde\xc0\xad\xde\xbe\xba\xfe\xca"
end_mark = b"\xbe\xba\xfe\xca\xde\xc0\xad\xde\x05\xca\xde\xc0\xbb\xc3\xe1\x1c"
start = c.find(start_mark) + len(start_mark)
end = c.find(end_mark)

rol = lambda val, r_bits, max_bits: \
	(val << r_bits % max_bits) & (2 ** max_bits - 1) | \
	((val & (2 ** max_bits - 1)) >> (max_bits - (r_bits % max_bits)))

ror = lambda val, r_bits, max_bits: \
	((val & (2 ** max_bits - 1)) >> r_bits % max_bits) | \
	(val << (max_bits - (r_bits % max_bits)) & (2 ** max_bits - 1))

print("Start: 0x%x" % start)
print("End: 0x%x" % end)

DEBUG = False

def gen_rand(i):
	if DEBUG:
		print('*************************')
		print('Addr: ' + hex(i))
	a = (65793 * (i & 0xff)) & 0xffffffff
	if DEBUG:
		print('A: ' + hex(a))
	r = (a + 4282663) & 0xffffffff
	if DEBUG:
		print('B: ' + hex(r))
	counts = bin(r).count("1")
	if DEBUG:
		print('Num of bits: ' + hex(counts))
	if i % 2 == 0:
		r = rol(r, counts, 8)
		if DEBUG:
			print('Result (ROL): ' + hex(r))
	else:
		r = ror(r, counts, 8)
		if DEBUG:
			print('Result (ROR): ' + hex(r))
	r = r % 0xa
	if DEBUG:
		print('Result mod 0xA: ' + hex(r))
	return r

def obf0(b, addr):
	return b ^ 0xcc

def obf1(b, addr):
	return (b + 0xaa) & 0xff

def obf2(b, addr):
	nb = (b - 0x42) & 0xff
	nb = rol(nb, 2, 8)
	return nb
	
def obf3(b, addr):
	return b ^ (addr & 0xff)
	
def obf4(b, addr):
	return (b + (addr & 0xff)) & 0xff
	
def obf5(b, addr):
	return (b - (addr & 0xff)) & 0xff

def obf6(b, addr):
	nb = ror(b, 4, 8)
	return ~nb & 0xff

def obf7(b, addr):
	return (~b ^ 0x17) & 0xff

def obf8(b, addr):
	return (~b ^ ~(addr & 0xff)) & 0xff

def obf9(b, addr):
	n_addr = rol(addr, 6, 8)
	n_addr = (~n_addr) & 0xff
	nb = b ^ n_addr
	nb = rol(nb, 3, 8)
	return nb & 0xff

cl = list(c)
routines = list()
for i in range(10):
	routines.append((1, "obf" + str(i)))

for i in range(start, end):
	v = i & 0xFF
	type = gen_rand(v)

	# get the routine to executed
	for j in range(10):
		index = (type+j) % 10
		(enabled, routine) = routines[index]
		if enabled:
			routines[index] = (0, routine)
			break
	else:
		print("Do wrap")
		for j in range(10):
			(enabled, tmp_routine) = routines[j]
			routines[j] = (1, tmp_routine)
		(enabled, routine) = routines[type]
		routines[type] = (0, routine)

	obf_b = locals()[routine](cl[i], i)
	print("Addr 0x%x (rand: 0x%x), orig byte 0x%x, obfuscated byte 0x%x obfuscation routine: %s" % (v, type, cl[i], obf_b, routine))
	cl[i] = obf_b

c = bytearray(cl)
file = os.path.basename(sys.argv[1])
dirname = os.path.dirname(sys.argv[1])
dest_file = os.path.join(dirname, 'obf_' + file)
with open(dest_file, 'wb') as f:
	f.write(c)