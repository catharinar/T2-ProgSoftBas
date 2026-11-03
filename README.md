# T2 - Programacao de Software Basico

Implementacao dos itens **3, 5 e 6**, considerando a ordem das funcoes
da secao 2.2 do enunciado. Os itens 1 (`mymemory_init`), 2
(`mymemory_alloc`) e 4 (`mymemory_display`) ficam com a dupla e ainda
precisam ser integrados. Este repositorio ainda nao e o trabalho completo.

## O que esta implementado

- `mymemory_free`: procura o endereco exato na lista, desconecta o nodo e
  libera apenas esse nodo. Ponteiros invalidos, internos a um bloco ou
  nulos sao ignorados. O espaco do pool fica disponivel para o alocador.
- `mymemory_stats`: mostra quantidade de alocacoes, bytes usados, bytes
  livres, maior intervalo livre e quantidade de intervalos livres.
  Considera inicio, intervalos entre blocos e fim do pool. Um pool vazio
  de tamanho positivo tem um fragmento; um pool cheio tem zero.
- `mymemory_cleanup`: salva o proximo nodo antes de liberar o atual,
  percorre toda a lista e libera o pool e a estrutura principal.

## Como explicar o codigo

Em `free`, `link` aponta para o ponteiro que leva ao nodo atual: primeiro
`head`, depois o campo `next` do nodo anterior. Fazer `*link = current->next`
remove o nodo inclusive quando ele e a cabeca, sem um caso especial.
Nao se chama `free(ptr)`, porque `ptr` aponta para dentro de um unico pool.
Essa busca custa O(n), com O(1) de memoria extra.

Em `stats`, a diferenca entre ponteiros `unsigned char *` fornece a posicao
em bytes. Para cada bloco, procura-se o menor inicio maior que o inicio
dele. A diferenca entre esse inicio (ou o fim do pool) e o fim do bloco
atual e uma lacuna. Tambem se conta a lacuna anterior ao primeiro bloco.
A busca custa O(n^2), com O(1) de memoria extra, e aceita listas fora de
ordem sem modifica-las. Uma lista garantidamente ordenada permitiria O(n).

Em `cleanup`, os nodos sao liberados antes do pool e da estrutura que
guarda os ponteiros. O custo e O(n). Depois da chamada, o chamador deve
fazer `memory = NULL` e nao utilizar ponteiros antigos.

## Integracao com a dupla

O alocador deve manter apenas alocacoes ativas, de tamanho positivo,
sem sobreposicao e inteiramente dentro do pool. Os nodos devem ser
alocados separadamente do pool. A lista nao precisa estar ordenada.
Quando um nodo e removido, o alocador precisa reconhecer a lacuna livre.

Adicionar as tres funcoes da dupla em `mymemory.c` ou em outro arquivo C
e compilar ambos, sem duplicar definicoes. O cabecalho fornecido foi mantido.
No exemplo de main, substituir `mymemory_malloc` por `mymemory_alloc` e
`mymemory_release` por `mymemory_cleanup`; verificar retornos NULL antes
de usar os ponteiros.

## Testes isolados

As fixtures montam nodos manualmente apenas para testar esta parte.
Nao implementam a estrategia de alocacao. Com GCC ou Clang, na raiz:

```sh
mkdir build
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -I. mymemory.c tests/test_mymemory.c -o build/test_mymemory
python tests/check_output.py ./build/test_mymemory
```

No Windows, usar `build/test_mymemory.exe`. Os testes verificam lista
vazia, cheia, fora de ordem, lacunas nas bordas e entre blocos, remocao
de cabeca/meio/cauda, ponteiros invalidos, liberacao repetida, preservacao
de dados ativos, cleanup com nodos pendentes e 50 blocos de tamanhos variados.
Os testes completos de alocacao/reuso devem ser feitos apos a integracao.

## Uso de IA

Esta parte (funcoes 3, 5 e 6), os testes e esta explicacao foram preparados
com auxilio do Codex. Os integrantes devem revisar, compreender e conseguir
explicar a implementacao na apresentacao, conforme o enunciado.
