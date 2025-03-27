# Sistema de Empréstimo de Salas

## Descrição

Este projeto é um sistema de empréstimo de salas desenvolvido em C utilizando a biblioteca GTK para a interface gráfica. O sistema permite que usuários façam login, cadastrem novos usuários e reservem salas para datas específicas. Foi desenvolvido para ser utilizado pelos servidores e funcionários da *Universidade de Pernambuco (UPE)*

## Estrutura do Projeto

- `src/`: Contém os arquivos fonte do projeto.
  - `db_operations.c` e `db_operations.h`: Funções para operações com banco de dados SQLite.
  - `ui_handlers.c`: Funções de manipulação da interface e eventos.
  - `main.c`: Arquivo principal que inicializa a aplicação.
  - `assets/`: Contém recursos da aplicação.
    - `css/style.css`: Estilos CSS para a interface.
    - `images/`: Imagens utilizadas na aplicação.
  - `views/`: Arquivos da interface.
    - `interface.glade`: Arquivo de interface gráfica criado com Glade.
  - `scripts/`: Scripts utilitários.
    - `reset_db.sh`: Script para resetar o banco de dados.

- `data/`: Contém o banco de dados SQLite com dados de login e reservas.

- `CMakeLists.txt`: Configuração do sistema de build CMake.
- `README.md`: Este arquivo.

## Dependências

- GTK+ 3.20 ou superior
- SQLite3
- CMake 3.10 ou superior
- Compilador GCC

## Como Compilar

1. Certifique-se de ter as dependências instaladas no seu sistema:

```sh
# Ubuntu/Debian
sudo apt install build-essential cmake libgtk-3-dev libsqlite3-dev
```

2. Navegue até o diretório do projeto e crie um diretório de build:

```sh
mkdir -p build && cd build
```

3. Configure e compile o projeto usando CMake:

```sh
cmake ..
make
```

## Executando

```sh
# A partir do diretório build
./Sistema-Emprestimo-de-Salas
```

## Funcionalidades

- Autenticação de usuários
- Cadastro de novos usuários
- Reserva de salas para datas e horários específicos
- Visualização de reservas existentes
- Cancelamento de reservas
- Opção "lembrar de mim" para facilitar logins posteriores

## Manutenção do Banco de Dados

Para resetar o banco de dados para o estado inicial:

```sh
cd src/scripts
./reset_db.sh
```
