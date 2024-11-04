#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>

#define ROWS 6
#define COLS 7
#define TILE_SIZE 100
#define WINDOW_WIDTH (COLS * TILE_SIZE)
#define WINDOW_HEIGHT (ROWS * TILE_SIZE + 100)  // Altura extra para a peça suspensa
#define CIRCLE_RADIUS (TILE_SIZE / 2 - 10)
#define DROP_SPEED 60 // Velocidade da queda da peça
#define HIGHLIGHT_POSITION (TILE_SIZE / 2 - 50)

typedef enum {
    NONE = 0,
    PLAYER1,
    PLAYER2
} Player;

typedef struct {
    Player board[ROWS][COLS];
    Player currentPlayer;
    bool gameOver;
    Player winner;
    bool vsComputer;
    int selectedCol;
    bool isDropping;
    int dropRow;
    int dropY;
    SDL_Point winningSequence[4];  // Coordenadas das peças vencedoras
} Game;


// Funções principais do jogo
void initSDL(SDL_Window** window, SDL_Renderer** renderer);
void cleanupSDL(SDL_Window* window, SDL_Renderer* renderer);
void initGame(Game* game, bool vsComputer);
void drawBoard(SDL_Renderer* renderer, const Game* game);
bool dropPiece(Game* game, int col);
bool checkDirection(const Game* game, int row, int col, int dRow, int dCol, SDL_Point* sequence);
Player checkVictory(Game* game);
bool boardFull(const Game* game);
void switchPlayer(Game* game);
int computerMove(Game* game);
void playMove(Game* game, int col);
void showEndGameScreen(SDL_Renderer* renderer, Game* game, TTF_Font* font);
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color);
void displayTurn(SDL_Renderer* renderer, const Game* game, TTF_Font* font);
bool showRestartPrompt(SDL_Renderer* renderer, TTF_Font* font);
int findWinningMove(Game* game, Player player);
void drawButton(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect buttonRect, const char* buttonText);

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Inicializa SDL e SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Erro ao inicializar SDL ou SDL_ttf: %s\n", SDL_GetError());
        return 1;
    }

    TTF_Font* font;
#ifdef _WIN32
    font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24); // Caminho para Windows
#else
    font = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf", 24); // Caminho para Linux
#endif

    if (!font) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        // Trate o erro conforme necessário
    }

    initSDL(&window, &renderer);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); // Ativa VSync para sincronizar com o monitor

    Game game;
    initGame(&game, true); // Inicia o jogo no modo jogador vs computador

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        Uint32 startTick = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEMOTION && !game.gameOver && !game.isDropping && game.currentPlayer == PLAYER1) {
                game.selectedCol = e.motion.x / TILE_SIZE;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && !game.gameOver && !game.isDropping && game.currentPlayer == PLAYER1) {
                int col = e.button.x / TILE_SIZE;
                if (game.board[0][col] == NONE) {
                    game.isDropping = true;
                    game.dropRow = ROWS - 1;
                    while (game.dropRow >= 0 && game.board[game.dropRow][col] != NONE) {
                        game.dropRow--;
                    }
                    game.dropY = 50;
                }
            }
        }

        if (game.isDropping) {
            game.dropY += DROP_SPEED;
            if (game.dropY >= (game.dropRow * TILE_SIZE + 50)) {
                game.board[game.dropRow][game.selectedCol] = game.currentPlayer;
                game.isDropping = false;
                game.dropY = 0;

                Player winner = checkVictory(&game);
                if (winner != NONE) {
                    game.winner = winner;
                    game.gameOver = true;
                } else if (boardFull(&game)) {
                    game.gameOver = true;
                } else {
                    switchPlayer(&game);
                }
            }
        }

        if (game.vsComputer && game.currentPlayer == PLAYER2 && !game.gameOver && !game.isDropping) {
            SDL_Delay(100); // Reduz o delay para uma atualização mais dinâmica
            int col = computerMove(&game);
            playMove(&game, col);
        }

        drawBoard(renderer, &game);
        displayTurn(renderer, &game, font);

        if (game.gameOver) {
            SDL_Delay(500); // Aguarda uma pausa mais curta
            showEndGameScreen(renderer, &game, font);

            bool playAgain = showRestartPrompt(renderer, font);
            if (playAgain) {
                initGame(&game, game.vsComputer);
            } else {
                quit = true;
            }
        }

        Uint32 frameTime = SDL_GetTicks() - startTick;
        if (frameTime < 4) {
            SDL_Delay(4 - frameTime); // Limita a atualização real com base no tempo do frame atual
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
    cleanupSDL(window, renderer);
    return 0;
}


// Função para exibir uma mensagem de reinício do jogo com opções "Sim" e "Não".
bool showRestartPrompt(SDL_Renderer* renderer, TTF_Font* font) {
    const char* message = "Deseja jogar novamente?";

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* messageSurface = TTF_RenderText_Solid(font, message, textColor);
    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);

    int messageWidth = messageSurface->w;
    int messageHeight = messageSurface->h;
    SDL_FreeSurface(messageSurface);

    SDL_Rect messageRect = {WINDOW_WIDTH / 2 - messageWidth / 2, WINDOW_HEIGHT / 2 - 80, messageWidth, messageHeight};
    SDL_Rect yesButton = {WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2, 100, 50};
    SDL_Rect noButton = {WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2, 100, 50};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
    drawButton(renderer, font, yesButton, "Sim");
    drawButton(renderer, font, noButton, "Não");
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(messageTexture);  // Libera a textura após uso

    bool selecting = true;
    bool playAgain = false;
    while (selecting) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                selecting = false;
                playAgain = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                if (x >= yesButton.x && x <= (yesButton.x + yesButton.w) &&
                    y >= yesButton.y && y <= (yesButton.y + yesButton.h)) {
                    playAgain = true;
                    selecting = false;
                    }
                if (x >= noButton.x && x <= (noButton.x + noButton.w) &&
                    y >= noButton.y && y <= (noButton.y + noButton.h)) {
                    playAgain = false;
                    selecting = false;
                    }
            }
        }
    }
    return playAgain;
}

void showEndGameScreen(SDL_Renderer* renderer, Game* game, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color highlightColor = {255, 255, 255, 255};
    for (int i = 0; i < 4; ++i) {
        int x = game->winningSequence[i].x * TILE_SIZE + TILE_SIZE / 2;
        int y = game->winningSequence[i].y * TILE_SIZE + TILE_SIZE / 2 + 50;
        drawCircle(renderer, x, y, CIRCLE_RADIUS, highlightColor);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(1500);

    const char* message = game->winner == PLAYER1 ? "Jogador 1 venceu!" : "Computador venceu!";
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, message, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {WINDOW_WIDTH / 2 - textWidth / 2, WINDOW_HEIGHT / 2 + TILE_SIZE, textWidth, textHeight};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_RenderPresent(renderer);

    // Libera a superfície e textura após uso
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);

    SDL_Delay(3000);
}

// Função para exibir de quem é o turno na interface (do jogador ou do computador).
void displayTurn(SDL_Renderer* renderer, const Game* game, TTF_Font* font) {
    SDL_Rect messageRect = {0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &messageRect);

    SDL_Color textColor = {0, 0, 0, 255};
    const char* message = game->currentPlayer == PLAYER1 ? "Sua vez!" : "Vez do computador!";

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, message, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {10, WINDOW_HEIGHT - 45, textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_DestroyTexture(texture);  // Libera a textura após uso
    SDL_RenderPresent(renderer);
}



void initSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // (Opcional) Dica para garantir que o SDL tente usar o driver OpenGL para renderização acelerada
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    // Criação da janela
    *window = SDL_CreateWindow("Conecta 4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) {
        printf("Erro ao criar janela SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Criação do renderizador com aceleração de hardware
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Erro ao criar renderizador SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Verificação de que o renderizador está usando aceleração de hardware
    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(*renderer, &info) == 0 && (info.flags & SDL_RENDERER_ACCELERATED)) {
        printf("Renderizador com aceleração de hardware ativo.\n");
    } else {
        printf("Renderizador sem aceleração de hardware.\n");
    }
}


// Função para limpar e encerrar os recursos da SDL.
void cleanupSDL(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Função para inicializar o estado do jogo, incluindo a configuração do tabuleiro e do jogador inicial.
void initGame(Game* game, bool vsComputer) {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            game->board[row][col] = NONE;
        }
    }
    game->currentPlayer = PLAYER1;
    game->gameOver = false;
    game->winner = NONE;
    game->vsComputer = vsComputer;
    game->selectedCol = -1;
    game->isDropping = false;
    game->dropRow = 0;
    game->dropY = 0;
}

// Função auxiliar para desenhar um círculo colorido em uma posição específica (usada para desenhar peças).
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}


// Função para desenhar o tabuleiro e as peças, incluindo a peça suspensa e a animação de queda.
void drawBoard(SDL_Renderer* renderer, const Game* game) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Fundo azul do tabuleiro
    SDL_RenderClear(renderer);

    // Desenha a peça suspensa (onde o jogador pretende colocar a peça)
    if (game->selectedCol >= 0 && game->selectedCol < COLS && !game->isDropping) {
        int centerX = game->selectedCol * TILE_SIZE + TILE_SIZE / 2;
        SDL_Color highlightColor = (game->currentPlayer == PLAYER1) ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){255, 255, 0, 255};
        drawCircle(renderer, centerX, HIGHLIGHT_POSITION, CIRCLE_RADIUS, highlightColor);
    }

    // Desenha as peças no tabuleiro
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            int centerX = col * TILE_SIZE + TILE_SIZE / 2;
            int centerY = row * TILE_SIZE + TILE_SIZE / 2 + 50; // Desloca o tabuleiro para baixo

            SDL_Color color;
            if (game->board[row][col] == PLAYER1) {
                color = (SDL_Color){255, 0, 0, 255}; // Vermelho
            } else if (game->board[row][col] == PLAYER2) {
                color = (SDL_Color){255, 255, 0, 255}; // Amarelo
            } else {
                color = (SDL_Color){255, 255, 255, 255}; // Branco
            }
            drawCircle(renderer, centerX, centerY, CIRCLE_RADIUS, color);
        }
    }

    // Desenha a peça caindo (durante a animação de queda)
    if (game->isDropping) {
        int centerX = game->selectedCol * TILE_SIZE + TILE_SIZE / 2;
        SDL_Color dropColor = (game->currentPlayer == PLAYER1) ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){255, 255, 0, 255};
        drawCircle(renderer, centerX, game->dropY, CIRCLE_RADIUS, dropColor);
    }

    SDL_RenderPresent(renderer);
}

// Função para simular a queda de uma peça em uma coluna específica e atualizar o tabuleiro.
bool dropPiece(Game* game, int col) {
    if (col < 0 || col >= COLS || game->board[0][col] != NONE) {
        return false;
    }
    for (int row = ROWS - 1; row >= 0; --row) {
        if (game->board[row][col] == NONE) {
            game->board[row][col] = game->currentPlayer;
            return true;
        }
    }
    return false;
}

// Função para verificar uma sequência de quatro peças na mesma direção e armazenar a sequência vencedora.
bool checkDirection(const Game* game, int row, int col, int dRow, int dCol, SDL_Point* sequence) {
    Player start = game->board[row][col];
    if (start == NONE) return false;

    for (int i = 0; i < 4; ++i) {
        int newRow = row + i * dRow;
        int newCol = col + i * dCol;
        if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS || game->board[newRow][newCol] != start) {
            return false;
        }
        sequence[i] = (SDL_Point){newCol, newRow};  // Armazena a sequência
    }
    return true;
}

// Função para verificar se um jogador ganhou o jogo, retornando o jogador vencedor ou NONE se não houver vitória.
Player checkVictory(Game* game) {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (game->board[row][col] != NONE) {
                if (checkDirection(game, row, col, 1, 0, game->winningSequence) ||  // Vertical
                    checkDirection(game, row, col, 0, 1, game->winningSequence) ||  // Horizontal
                    checkDirection(game, row, col, 1, 1, game->winningSequence) ||  // Diagonal descendente
                    checkDirection(game, row, col, 1, -1, game->winningSequence))   // Diagonal ascendente
                {
                    return game->board[row][col];
                }
            }
        }
    }
    return NONE;
}

// Função para verificar se o tabuleiro está completamente preenchido, indicando um empate.
bool boardFull(const Game* game) {
    for (int col = 0; col < COLS; ++col) {
        if (game->board[0][col] == NONE) {
            return false;
        }
    }
    return true;
}

// Função para alternar entre os jogadores após uma jogada.
void switchPlayer(Game* game) {
    game->currentPlayer = (game->currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
}

// Função para realizar a jogada em uma coluna e verificar o estado do jogo após o movimento.
void playMove(Game* game, int col) {
    if (dropPiece(game, col)) {
        Player winner = checkVictory(game);
        if (winner != NONE) {
            game->winner = winner;
            game->gameOver = true;
        } else if (boardFull(game)) {
            game->gameOver = true;
        } else {
            switchPlayer(game);
        }
    }
}

// Função para a jogada do computador, incluindo estratégias de bloqueio e movimentos prioritários.
int computerMove(Game* game) {
    // 1. Verifica se o computador pode vencer na próxima jogada
    int winningMove = findWinningMove(game, PLAYER2);
    if (winningMove != -1) {
        return winningMove;  // Jogada de vitória encontrada
    }

    // 2. Verifica se o jogador pode vencer na próxima jogada e bloqueia
    int blockingMove = findWinningMove(game, PLAYER1);
    if (blockingMove != -1) {
        return blockingMove;  // Bloquear a jogada de vitória do jogador
    }

    // 3. Prioriza a coluna central para controle estratégico
    int centerColumn = COLS / 2;
    if (game->board[0][centerColumn] == NONE) {
        return centerColumn;
    }

    // 4. Procura colunas adjacentes ao centro para expandir o controle
    for (int offset = 1; offset <= COLS / 2; ++offset) {
        if (centerColumn - offset >= 0 && game->board[0][centerColumn - offset] == NONE) {
            return centerColumn - offset;
        }
        if (centerColumn + offset < COLS && game->board[0][centerColumn + offset] == NONE) {
            return centerColumn + offset;
        }
    }

    // 5. Verifica colunas laterais como último recurso
    for (int col = 0; col < COLS; ++col) {
        if (game->board[0][col] == NONE) {
            return col;
        }
    }

    return -1;  // Nenhuma coluna disponível
}

int findWinningMove(Game* game, Player player) {
    for (int col = 0; col < COLS; ++col) {
        // Simula colocar uma peça nesta coluna para verificar se cria uma vitória
        if (game->board[0][col] == NONE) {
            int row;
            for (row = ROWS - 1; row >= 0; --row) {
                if (game->board[row][col] == NONE) {
                    break;
                }
            }
            // Coloca a peça temporariamente
            game->board[row][col] = player;
            if (checkVictory(game) == player) {
                game->board[row][col] = NONE;  // Remove a peça temporária
                return col;  // Retorna a coluna vencedora ou de bloqueio
            }
            game->board[row][col] = NONE;  // Remove a peça temporária
        }
    }
    return -1;  // Nenhuma jogada de vitória ou bloqueio encontrada
}

void drawButton(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect buttonRect, const char* buttonText) {
    // Configura as cores do botão
    SDL_Color borderColor = {200, 200, 200, 255};    // Cinza claro para a borda
    SDL_Color fillColor = {100, 149, 237, 255};      // Azul claro para o fundo do botão
    SDL_Color textColor = {255, 255, 255, 255};      // Branco para o texto

    // Desenha o fundo do botão
    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Desenha a borda do botão
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &buttonRect);

    // Desenha o texto do botão
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, buttonText, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Centraliza o texto no botão
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {
        buttonRect.x + (buttonRect.w - textWidth) / 2,
        buttonRect.y + (buttonRect.h - textHeight) / 2,
        textWidth,
        textHeight
    };

    SDL_FreeSurface(textSurface);

    // Renderiza o texto
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}
