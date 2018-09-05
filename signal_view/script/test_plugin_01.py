def main():
	a = 1
	b = a * a
	return b


def hello():
	return 'hello123'


def add(a, b):
	return a + b


def process(kw):
	return str(kw['name']) + str(kw['value'])


# if __name__ == '__main__':
# 	kw = {'name': 'liuyang', 'value': 123}
# 	print(process(**kw))
