"""Confere os cinco valores de cada cenario emitido pelo executavel."""
import re
import subprocess
import sys

result = subprocess.run([sys.argv[1]], check=True, capture_output=True, text=True)
expected = {
    "vazio": [0, 0, 1000, 1000, 1],
    "fragmentado": [3, 400, 600, 200, 4],
    "cheio": [2, 1000, 0, 0, 0],
    "50_blocos": [50, 458, 542, 19, 50],
}
for name, values in expected.items():
    section = result.stdout.split(f"[{name}]\n", 1)[1].splitlines()[:5]
    actual = [int(re.search(r": (\d+)", line).group(1)) for line in section]
    assert actual == values, (name, actual, values)
assert result.stdout.endswith("OK\n")
print(result.stdout, end="")
print("Estatisticas conferidas: 4 cenarios OK.")
