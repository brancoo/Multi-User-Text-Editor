# SO_1819
Text Editor in C (Unix)

# Meta 1
 - [x] Definição de estruturas e costantes necessários ao editor de texto e utilizador;
 - [x] Função para verificar username num determinado ficheiro;
 - [x] Edição e navegação de uma linha;
 - [X] Leitura e validação dos comandos/argumentos opcionais do servidor;
 - [x] Leitura e validação das variáveis de ambiente do servidor e cliente.

# Meta 2
 - [x] Comunicação através de named piped entre servidor e cliente;
 - [x] Gestão dos clientes (detectar que existem, avisar que o servidor encerrou, etc) a funcionar;
 - [x] Login dos utilizadores a funcionar;
 - [] Lançamento e comunicação com o dicionário preparada, incluindo verificações.


# Bugs / A melhorar
 - Organização do código.
 - Implementar no servidor array de estruturas para os clientes para a comunicação entre o servidor e os clientes; (preciso para a 3ª meta);
 - Actualizar em tempo real o conteúdo do editor no lado do servidor;
 - Passar a variável content da estrutura Editar para um array 2D dinâmico.
