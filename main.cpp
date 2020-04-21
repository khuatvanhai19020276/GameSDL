/* GAME DÒ MÌN ---------------------------------------------------------------

LUẬT CHƠI:
    Có một bảng chứa một số quả mìn (mine).

    Mở một ô (cell) bằng cách click chuột trái vào ô đó. Nếu ô đó là mìn, bạn
    thua. Nếu không phải mìn, ô đó sẽ có giá trị từ 0 đến 8 thể hiện số quả
    mìn có trong 8 ô xung quanh. Nếu mở hết các ô không phải mìn, bạn thắng.

    Có thể click chuột phải vào ô nghi ngờ là mìn để gắn cờ (flag) đánh dấu.

CÀI ĐẶT:
    MacOS: brew install sdl2 sdl2_image

BIÊN DỊCH:
    MacOS: g++ minesweeper.cpp -o minesweeper -lsdl2 -lsdl2_image
*/



/* Khai báo thư viện ------------------------------------------------------ */



#include <vector>
#include <string>
#include <ctime>                // Hàm time
#include <cstdlib>              // Hàm rand, srand
#include <SDL.h>           // Tạo cửa sổ, vẽ vào cửa sổ
#include <SDL_image.h>     // Load ảnh


using std::vector;
using std::string;



/* Hằng ------------------------------------------------------------------- */



const int MINE_VALUE            = 9;    // Giá trị thể hiện một ô đang có mìn

const int SPRITE_CELL_WIDTH     = 16;   // Chiều rộng gốc của một ô
const int SPRITE_CELL_HEIGHT    = 16;   // Chiều cao gốc của một ô

const int WINDOW_CELL_WIDTH     = 32;   // Chiều rộng khi hiển thị của một ô
const int WINDOW_CELL_HEIGHT    = 32;   // Chiều cao khi hiển thị của một ô

const int DEFAULT_NUM_ROWS      = 9;    // Số lượng ô theo chiều ngang
const int DEFAULT_NUM_COLS      = 9;    // Số lượng ô theo chiều dọc
const int DEFAULT_NUM_MINES     = 10;   // Số lượng mìn

const string SCREEN_TITLE       = "Mine sweeper";   // Tiêu đề cửa sổ
const string SPRITE_PATH        = "sprite.png";     // Đường dẫn đến file ảnh



/* Kiểu dữ liệu mới ------------------------------------------------------- */



enum CellState {        // Trạng thái của một ô
    CELL_HIDDEN,        // Ẩn
    CELL_SHOWN,         // Hiện
    CELL_FLAGGED        // Gắn cờ
};

enum GameState {        // Trạng thái của trò chơi
    GAME_PLAYING,       // Đang chơi
    GAME_WON,           // Đã thắng
    GAME_LOST           // Đã thua
};

enum SpriteType {       // Các loại hình sử dụng
    SPRITE_ZERO,
    SPRITE_ONE,
    SPRITE_TWO,
    SPRITE_THREE,
    SPRITE_FOUR,
    SPRITE_FIVE,
    SPRITE_SIX,
    SPRITE_SEVEN,
    SPRITE_EIGHT,

    SPRITE_MINE,        // Hình mìn thường
    SPRITE_MINE_CURRENT,// Hình mìn đỏ
    SPRITE_FLAG,
    SPRITE_HIDDEN,

    SPRITE_TOTAL,       // Tổng số hình sử dụng
};

struct Cell {           // Đại diện cho một ô trong bảng
    int value;          // Giá trị một ô (mìn hoặc số lượng mìn xung quanh)
    CellState state;    // Trạng thái một ô (ẩn/hiện/gắn cờ)
};

struct CellPos {        // Đại diện cho vị trí của một ô trong bảng
    int row;            // Hàng, tính từ trên, bắt đầu từ 0
    int col;            // Cột, tính từ trái, bắt đầu từ 0
};

typedef vector<vector<Cell> > CellTable;// Bảng

struct Game {           // Đại diện cho một game
    int nRows;          // Số hàng
    int nCols;          // Số cột
    int nMines;         // Số mìn
    int nShownCells;    // Số lượng ô đã mở
    CellTable cells;    // Các ô trong bảng
    CellPos lastPos;    // Vị trí ô cuối cùng mở
    GameState state;    // Trạng thái của game (chơi/thắng/thua)
};

struct Graphic {
    SDL_Window *window;                 // Cửa sổ hiển thị
    SDL_Texture *spriteTexture;         // Ảnh load được từ file sprite.png
    vector<SDL_Rect> spriteRects;       // Vị trí các hình trong file ảnh
    SDL_Renderer *renderer;             // Dùng để vẽ hình vào cửa sổ

};



/* Nguyên mẫu hàm --------------------------------------------------------- */



// Khởi tạo SDL, SDL_Image, tạo cửa sổ, load ảnh, ...  Trả về false nếu khởi
// tạo không thành công
bool initGraphic(Graphic &g, int nRows, int nCols);

// Huỷ khởi tạo những gì đã dùng từ SDL và SDL_Image
void finalizeGraphic(Graphic &g);

// Load ảnh từ đường dẫn path, trả về ảnh biểu diễn dưới dạng SDL_Texture
SDL_Texture* createTexture(SDL_Renderer *renderer, const string &path);

// Lưu vị trí các hình trong ảnh vào tham số rects
void initSpriteRects(vector<SDL_Rect> &rects);

// Khởi tạo game, mặc định các ô có trạng thái CELL_HIDDEN, giá trị các ô lấy
// từ hàm randomMines và countMines
void initGame(Game &game, int nRows, int nCols, int nMines);

// Sinh ngẫu nhiên vị trí các quả mìn
vector<CellPos> randomMines(int nRows, int nCols, int nMines);

// Đếm số mìn xung quanh một ô ở vị trí pos
int countMines(const CellTable &cells, const CellPos &pos);

// Hiển thị trạng thái game hiện tại lên cửa sổ
void displayGame(const Game &game, const Graphic &graphic);

// Lấy hình sẽ vẽ cho ô ở vị trí pos
SDL_Rect getSpriteRect(const Game &game, const CellPos &pos,
                       const vector<SDL_Rect> &spriteRects);

// Cập nhật trạng thái của game khi có sự kiện event
void updateGame(Game &game, const SDL_Event& event);

// Thay đổi trạng thái một ô (CELL_FLAGGED <-> CELL_HIDDEN)
void toggleCellState(Cell &cell);

// Đánh dấu các ô là CELL_SHOWN khi chọn mở ô ở vị trí pos. Trả về số lượng
// các ô vừa mở
int showCells(CellTable &cells, const CellPos &pos);

// Hiển thị cửa sổ thông báo lỗi
void err(const string &msg);



/* Định nghĩa hàm --------------------------------------------------------- */



int main(int argc, char* argv[] ) {
    srand(time(0)); // Khởi tạo seed cho hàm rand

    int nRows = DEFAULT_NUM_ROWS,
        nCols = DEFAULT_NUM_COLS,
        nMines = DEFAULT_NUM_MINES;

    Graphic graphic;
    if (!initGraphic(graphic, nRows, nCols)) {
        finalizeGraphic(graphic);
        return EXIT_FAILURE;
    }

    Game game;
    initGame(game, nRows, nCols, nMines);

    bool quit = false;
    while (!quit) {
        displayGame(game, graphic);

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
                break;
            }

            updateGame(game, event);
        }
    }

    finalizeGraphic(graphic);
    return EXIT_SUCCESS;
}

bool initGraphic(Graphic &g, int nRows, int nCols) {
    g.window = NULL;
    g.renderer = NULL;
    g.spriteTexture = NULL;

    int sdlFlags = SDL_INIT_VIDEO;  // Dùng SDL để hiển thị hình ảnh
    if (SDL_Init(sdlFlags) != 0) {
        err("SDL could not initialize!");
        return false;
    }

    int imgFlags = IMG_INIT_PNG;    // Dùng SDL_Image để load ảnh png
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        err("SDL_Image could not initialize!");
        return false;
    }

    g.window = SDL_CreateWindow(SCREEN_TITLE.c_str(),       // Tiêu đề
                                SDL_WINDOWPOS_UNDEFINED,    // x
                                SDL_WINDOWPOS_UNDEFINED,    // y
                                nCols * WINDOW_CELL_WIDTH,  // Chiều rộng
                                nRows * WINDOW_CELL_HEIGHT, // Chiều cao
                                SDL_WINDOW_SHOWN);          // Hiển thị

    if (g.window == NULL) {
        err("Window could not be created!");
        return false;
    }

    g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED);
    if (g.renderer == NULL) {
        err("Renderer could not be created!");
        return false;
    }

    g.spriteTexture = createTexture(g.renderer, SPRITE_PATH);
    if (g.spriteTexture == NULL) {
        err("Unable to create texture from " + SPRITE_PATH + "!");
        return false;
    }

    initSpriteRects(g.spriteRects);
    return true;
}

void finalizeGraphic(Graphic &g) {
    SDL_DestroyTexture(g.spriteTexture);
    SDL_DestroyRenderer(g.renderer);
    SDL_DestroyWindow(g.window);

    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* createTexture(SDL_Renderer *renderer, const string &path) {
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (surface == NULL) {
        err("Unable to load image " + path + "!");
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void initSpriteRects(std::vector<SDL_Rect> &rects) {
    for (int i = 0; i < SPRITE_TOTAL; i ++) {
        SpriteType type = (SpriteType) i;
        SDL_Rect rect = {0, 0, SPRITE_CELL_WIDTH, SPRITE_CELL_HEIGHT};
        if (SPRITE_ZERO <= type && type <= SPRITE_EIGHT) {
            rect.x = i * SPRITE_CELL_WIDTH;
        } else {
            int col;
            switch (type) {
                case SPRITE_MINE:
                    col = 0;
                    break;
                case SPRITE_MINE_CURRENT:
                    col = 2;
                    break;
                case SPRITE_FLAG:
                    col = 4;
                    break;
                case SPRITE_HIDDEN:
                    col = 5;
                    break;
                default:
                    col = 5;
                    break;
            }
            rect.x = col * SPRITE_CELL_WIDTH;
            rect.y = SPRITE_CELL_HEIGHT;
        }
        rects.push_back(rect);
    }
}

void initGame(Game &game, int nRows, int nCols, int nMines) {
    game.cells = CellTable(nRows, vector<Cell>(nCols));
    for (int i = 0; i < nRows; i ++) {
        for (int j = 0; j < nCols; j ++) {
            game.cells[i][j] = (Cell) {0, CELL_HIDDEN};
        }
    }

    vector<CellPos> minePos = randomMines(nRows, nCols, nMines);
    for (int i = 0; i < minePos.size(); i ++) {
        int row = minePos[i].row;
        int col = minePos[i].col;
        game.cells[row][col].value = MINE_VALUE;
    }

    for (int i = 0; i < nRows; i ++) {
        for (int j = 0; j < nCols; j ++) {
            if (game.cells[i][j].value != MINE_VALUE) {
                CellPos pos = {i, j};
                game.cells[i][j].value = countMines(game.cells, pos);
            }
        }
    }

    game.nRows = nRows;
    game.nCols = nCols;
    game.nMines = nMines;
    game.state = GAME_PLAYING;
    game.nShownCells = 0;
}

vector<CellPos> randomMines(int nRows, int nCols, int nMines) {
    int maxVal = nRows * nCols;

    vector<CellPos> minePos;
    vector<bool> chosenVals(maxVal, false);

    while (minePos.size() < nMines) {
        int val = rand() % maxVal;
        if (!chosenVals[val]) {
            chosenVals[val] = true;
            int row = val / nCols;
            int col = val % nCols;
            minePos.push_back((CellPos) {row, col});
        }
    }
    return minePos;
}

int countMines(const CellTable &cells, const CellPos &pos) {
    int cnt = 0;
    int nRows = cells.size();
    int nCols = cells[0].size();

    for (int i = -1; i <= 1; i ++) {
        for (int j = -1; j <= 1; j ++) {
            int y = pos.row + i;
            int x = pos.col + j;

            if (x >= 0 && x < nCols && y >= 0 && y < nRows
                && cells[y][x].value == MINE_VALUE) {
                cnt ++;
            }
        }
    }
    return cnt;
}

void displayGame(const Game &game, const Graphic &graphic) {
    SDL_RenderClear(graphic.renderer);      // Xoá những gì đã vẽ

    for (int i = 0; i < game.nRows; i ++) {
        for (int j = 0; j < game.nCols; j ++){
            SDL_Rect destRect = {           // Vị trí sẽ vẽ trên cửa sổ
                j * WINDOW_CELL_WIDTH,
                i * WINDOW_CELL_HEIGHT,
                WINDOW_CELL_WIDTH,
                WINDOW_CELL_HEIGHT
            };

            CellPos pos = {i, j};

            // Vị trí lấy từ file ảnh để vẽ
            SDL_Rect srcRect = getSpriteRect(game, pos, graphic.spriteRects);

            // Vẽ
            SDL_RenderCopy(graphic.renderer, graphic.spriteTexture, &srcRect,
                           &destRect);
        }
    }
    SDL_RenderPresent(graphic.renderer);    // Hiển thị những gì vữa vẽ
}

SDL_Rect getSpriteRect(const Game &game, const CellPos &pos,
                       const vector<SDL_Rect> &spriteRects) {

    Cell cell = game.cells[pos.row][pos.col];
    if (game.state == GAME_PLAYING) {
        switch (cell.state) {
            case CELL_HIDDEN:
                return spriteRects[SPRITE_HIDDEN];
            case CELL_FLAGGED:
                return spriteRects[SPRITE_FLAG];
            case CELL_SHOWN:
                return spriteRects[cell.value];
        }
    } else if (game.state == GAME_WON) {
        if (0 <= cell.value && cell.value <= 8) {
            return spriteRects[cell.value];
        } else if (cell.value == MINE_VALUE) {
            return spriteRects[SPRITE_FLAG];
        }
    } else if (game.state == GAME_LOST) {
        if (0 <= cell.value && cell.value <= 8) {
            if (cell.state == CELL_HIDDEN) {
                return spriteRects[SPRITE_HIDDEN];
            } else {
                return spriteRects[cell.value];
            }
        } else if (cell.value == MINE_VALUE) {
            int lastRow = game.lastPos.row;
            int lastCol = game.lastPos.col;
            if (pos.row == lastRow && pos.col == lastCol) {
                return spriteRects[SPRITE_MINE_CURRENT];
            } else {
                return spriteRects[SPRITE_MINE];
            }
        }
    }

    return spriteRects[SPRITE_ZERO];            // Sẽ không vào lệnh này
}

void updateGame(Game &game, const SDL_Event &event) {
    if (game.state != GAME_PLAYING) {
        return;
    }

    if (event.type != SDL_MOUSEBUTTONDOWN) {    // Sự kiện click chuột
        return;
    }

    SDL_MouseButtonEvent mouse = event.button;
    int row = mouse.y / WINDOW_CELL_HEIGHT,
        col = mouse.x / WINDOW_CELL_WIDTH;

    game.lastPos = (CellPos) {row, col};

    Cell &cell = game.cells[row][col];
    if (cell.state == CELL_SHOWN) {
        return;
    }

    if (mouse.button == SDL_BUTTON_RIGHT) {     // Click chuột phải
        toggleCellState(cell);
        return;
    }

    if (cell.value == MINE_VALUE) {
        game.state = GAME_LOST;
        return;
    }

    int shown = showCells(game.cells, game.lastPos);
    game.nShownCells += shown;

    if (game.nShownCells == game.nRows * game.nCols - game.nMines) {
        game.state = GAME_WON;
    }
}

void toggleCellState(Cell &cell) {
    switch (cell.state) {
        case CELL_FLAGGED:
            cell.state = CELL_HIDDEN;
            break;
        case CELL_HIDDEN:
            cell.state = CELL_FLAGGED;
            break;
        default:
            break;
    }
}

int showCells(CellTable &cells, const CellPos &pos) {
    Cell &cell = cells[pos.row][pos.col];

    cell.state = CELL_SHOWN;
    if (cell.value != 0) {
        return 1;   // Mở được một ô
    }

    int nRows = cells.size();
    int nCols = cells[0].size();

    int nShownCells = 1;

    // Vì ô hiện tại có value là 0 nên mở cả 8 ô xung quanh luôn
    for (int i = -1; i <= 1; i ++) {
        for (int j = -1; j <= 1; j ++) {
            int y = pos.row + i;
            int x = pos.col + j;

            // Chỉ mở ô hợp lệ và đang chưa được mở
            if (x >= 0 && x < nCols && y >= 0 && y < nRows
                && cells[y][x].state != CELL_SHOWN) {
                CellPos nextPos = {y, x};

                // Mở ô nextPos (và có thể sẽ mở cả 8 ô xung quanh ô đó)
                nShownCells += showCells(cells, nextPos);
            }
        }
    }
    return nShownCells;
}

void err(const string &m) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", m.c_str(), NULL);
}



/* Hết -------------------------------------------------------------------- */
