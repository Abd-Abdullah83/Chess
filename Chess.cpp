
#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Audio.hpp>
#include <cctype>
using namespace sf;
using namespace std;



SoundBuffer moveBuffer;
SoundBuffer startBuffer;
SoundBuffer completingBuffer;


Sound moveSound;
Sound startSound;
Sound completingSound;



// =====================================================
//  CHESS LOGIC
// =====================================================

const int SIZE = 8;
char boardLogic[SIZE][SIZE];
bool whiteTurn = true;


bool isValidMove(int sx, int sy, int dx, int dy);
bool isValidPawnMove(int sx, int sy, int dx, int dy, char piece);
bool isvalidRookmove(int sx, int sy, int dx, int dy, char p);
bool isvalidKinghtmove(int sx, int sy, int dx, int dy, char p);
bool isValidBishopMove(int sx, int sy, int dx, int dy, char p);
bool isValidQueenMove(int sx, int sy, int dx, int dy, char p);
bool isvalidKingmove(int sx, int sy, int dx, int dy, char p);

void findKing(char kingChar, int& kx, int& ky);
bool isSquareAttacked(int dx, int dy, bool byWhite);
bool isInCheck(bool whiteTurn);
bool hasAnyLegalMove(bool turn);
bool isCheckmate(bool whiteTurn);
bool isStalemate(bool whiteTurn);
bool kingExists(bool white);



bool isInsideBoard(int r, int c)
{
    if (r >= 0 && r < 8 && c >= 0 && c < 8) // tackle the Pieces must not go outside board
        return true;

    return false;
}
void initializeBoardLogic()            //Set Character Board
{

    for (int row = 0; row < SIZE; row++)
    {
        for (int col = 0; col < SIZE; col++)
        {
            boardLogic[row][col] = ' ';
        }
    }
    for (int i = 0; i < SIZE; i++)
    {
        boardLogic[6][i] = 'P'; // White-pawn at 2nd Row from Bottom
        boardLogic[1][i] = 'p'; // Black-pawn at 2nd Row from Top
    }
    // for White Pieces (Upper-Case)
    boardLogic[7][4] = 'K';
    boardLogic[7][5] = 'B';
    boardLogic[7][6] = 'N';
    boardLogic[7][7] = 'R';
    boardLogic[7][0] = 'R';
    boardLogic[7][1] = 'N';
    boardLogic[7][2] = 'B';
    boardLogic[7][3] = 'Q';
    //Black Pieces (Lower-case)
    boardLogic[0][0] = 'r';
    boardLogic[0][1] = 'n';
    boardLogic[0][2] = 'b';
    boardLogic[0][3] = 'q';
    boardLogic[0][4] = 'k';
    boardLogic[0][5] = 'b';
    boardLogic[0][6] = 'n';
    boardLogic[0][7] = 'r';
}
bool isValidMove(int sx, int sy, int dx, int dy)
{
    if (!isInsideBoard(dx, dy))                                          //Destination Box Must be inside Board 8*8
    {
        return false;
    }

    char piece = boardLogic[sx][sy];                                     //Check if the source square has a piece or not(empty)
    if (piece == ' ')
    {
        return false;
    }

    
    if (whiteTurn && islower(piece))                                      //Check whether Valid Turn or Not
    {
        return false;
    }
    if (!whiteTurn && isupper(piece))
    {
        return false;
    }

    switch (tolower(piece))                                               //Check if the move is valid according to the piece type
    {
    case 'p':
        return isValidPawnMove(sx, sy, dx, dy, piece);
    case 'r':
        return isvalidRookmove(sx, sy, dx, dy, piece);
    case 'n':
        return isvalidKinghtmove(sx, sy, dx, dy, piece);
    case 'b':
        return isValidBishopMove(sx, sy, dx, dy, piece);
    case 'q':
        return isValidQueenMove(sx, sy, dx, dy, piece);
    case 'k':
        return isvalidKingmove(sx, sy, dx, dy, piece);
    }
    return false;
}
bool isValidPawnMove(int sx, int sy, int dx, int dy, char piece)
{
    int dir = 0;
    if (isupper(piece)) // White up(-1), black down(+1)
    {
        dir = -1;
    }
    else
    {
        dir = 1;
    }

    if (dx == sx + dir && dy == sy && boardLogic[dx][dy] == ' ') //condition for one move
        return true;

    if ((piece == 'P' && sx == 6) || (piece == 'p' && sx == 1)) // for White pawn at initial position row 6 and Black pawn at initial position row 1
    {
        if (dx == sx + 2 * dir && dy == sy &&  // move exactly 2 rows forward and must be in same column
            boardLogic[sx + dir][sy] == ' ' && // square in front must be empty
            boardLogic[dx][dy] == ' ')    // destination must be empty
        {
            return true;
        }
    }

    if (dx == sx + dir && abs(dy - sy) == 1 &&          //for capturing (not same side piece) && destination !=space
        boardLogic[dx][dy] != ' ' &&
        isupper(piece) != isupper(boardLogic[dx][dy]))
        return true;

    return false;
} 
bool isvalidRookmove(int sx, int sy, int dx, int dy, char piece)
{
    if (dx != sx && dy != sy) // for same row because weather the row or column should be same
        return false;
    int stepX = 0, stepY = 0;
    if (dx > sx) // if moving down dx > sx (0 >> 7) stepX should be increase
    {
        stepX = +1;
    }
    else if (sx > dx) // if moving up sx > dx (7 >> 0) stepX should be decrease
    {
        stepX = -1;
    }
    else if (dy > sy) // if moving right then dy > sy (0 >> 7) stepY should be increase
    {
        stepY = +1;
    }
    else if (sy > dy) // if moving left then sy > dy (7 >> 0) stepY should be Decrease
    {
        stepY = -1;
    }
    int x = sx + stepX, y = sy + stepY;
    while (y != dy || x != dx) // check one coordinate before the destination
    {
        if (boardLogic[x][y] != ' ') // checking for whether the path is block or not 
        {
            return false;
        }
        x += stepX, y += stepY; // increment or decrement for next checking
    }
    if (boardLogic[dx][dy] == ' ') // if not space then return true for making move
    {
        return true;
    }
    if (isupper(piece) != isupper(boardLogic[dx][dy]))
    {
        return true;
    }
    return false;
}
bool isValidBishopMove(int sx, int sy, int dx, int dy, char piece)
{
    if (abs(dx - sx) != abs(dy - sy)) // change in row == change in column for bishop (abs function make - to +)
    {
        return false;
    }

    int stepX = (dx > sx) ? 1 : -1; //down Right x+=1, down left x+=1, Upward (Right or left) x-=1
    int stepY = (dy > sy) ? 1 : -1; //down Right y+=1, down left y-=1, Upward Right y+=1, Upward left y-=1

    int x = sx + stepX;
    int y = sy + stepY;


    while (x != dx && y != dy) // Checking for the move (not blocked, is-space )
    {
        if (boardLogic[x][y] != ' ')
        {
            return false;
        }
        x += stepX;
        y += stepY;
    }


    if (boardLogic[dx][dy] == ' ' || (isupper(piece) != isupper(boardLogic[dx][dy]))) //  if !same piece or not space then return true for making move
    {
        return true;
    }

    return false;
}
bool isvalidKinghtmove(int sx, int sy, int dx, int dy, char piece)
{
    int x = abs(dx - sx), y = abs(dy - sy);
    if (!((x == 2 && y == 1) || (x == 1 && y == 2)))                            //knight can move L shape within the Board
    {                                                                           // so row =2 and column =1 or vise versa
        return false;
    }
    if (boardLogic[dx][dy] == ' ' || (isupper(piece) != isupper(boardLogic[dx][dy])))
    {
        return true;
    }
    return false;

}
bool isValidQueenMove(int sx, int sy, int dx, int dy, char piece)
{
    //Queen can move Digonally or in a straight line
    if (abs(dx - sx) == abs(dy - sy))                                        // so I have a function of Rook and Bishop
    {                                                                        // for Rook atleast same row or column
        return isValidBishopMove(sx, sy, dx, dy, piece);                     // for bishop change in row == change in column
    }                                                                        // if destination is not as same as rook or bishop then not valid move

    else if (sx == dx || sy == dy)
    {
        return isvalidRookmove(sx, sy, dx, dy, piece);
    }
    else
    {
        return false;
    }
}
bool isvalidKingmove(int sx, int sy, int dx, int dy, char piece)
{
    int x = abs(sx - dx), y = abs(dy - sy);
    if (x == 0 && y == 0) //
    {
        return false;
    }
    if (x > 1 || y > 1)                                                 // King can move to consective box only so 
    {                                                                   //So x and y can't be Exceed by 1
        return false;                                                   // And Both cant be same at the same time
    }
    if (boardLogic[dx][dy] == ' ' || (islower(piece) != islower(boardLogic[dx][dy])))
    {
        return true;
    }

    return false;
}
void makeMove(int sx, int sy, int dx, int dy)
{

    boardLogic[dx][dy] = boardLogic[sx][sy];                                          //saving Piece in Destination 
    boardLogic[sx][sy] = ' ';                                                         // Saving space at Source Location
}
void findKing(char king, int& kx, int& ky)    //This Function Search for King Position on the Board
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (boardLogic[r][c] == king)
            {
                kx = r;
                ky = c;
                return;
            }
        }
    }
}



bool isSquareAttacked(int dx, int dy, bool byWhite)
{

    bool old = whiteTurn;
    whiteTurn = byWhite;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            char p = boardLogic[r][c];
            if (p == ' ')
                continue;

            if (byWhite && (isupper(p) == false))
                continue;
            if (!byWhite && isupper(p) == true)
                continue;

            if (isValidMove(r, c, dx, dy))
            {
                whiteTurn = old;
                return true;
            }
        }
    }
    whiteTurn = old;
    return false;
}

bool isInCheck(bool whiteChecked)
{
    int kingX = -1, kingY = -1;

    if (whiteChecked)                // WhiteChecked Provides the detail which King is actually Attacked
        findKing('K', kingX, kingY); //Find the Location of White King if it is checked
    else
        findKing('k', kingX, kingY); //Find the Location of Black King if it is checked

    if (kingX == -1)
        return false; // safety

    return isSquareAttacked(kingX, kingY, !whiteChecked); // Check the King Square is Under Attack or Not
}

bool hasAnyLegalMove(bool turn)
{
    // Make sure isValidMove and isInCheck work for the side we're testing by
    // temporarily setting the global whiteTurn flag.
    bool old = whiteTurn;
    whiteTurn = turn;

    for (int sx = 0; sx < 8; sx++)
    {
        for (int sy = 0; sy < 8; sy++)
        {
            char p = boardLogic[sx][sy];
            if (p == ' ') continue;

            if (turn && islower(p))
                continue;
            if (!turn && isupper(p))
                continue;

            for (int dx = 0; dx < 8; dx++)
            {
                for (int dy = 0; dy < 8; dy++)
                {
                    if (isValidMove(sx, sy, dx, dy))
                    {
                        // simulate
                        char backup = boardLogic[dx][dy];
                        char self = boardLogic[sx][sy];

                        boardLogic[dx][dy] = self;
                        boardLogic[sx][sy] = ' ';

                        bool stillCheck = isInCheck(turn);

                        // undo
                        boardLogic[sx][sy] = self;
                        boardLogic[dx][dy] = backup;

                        if (!stillCheck)
                        {
                            whiteTurn = old;
                            return true;
                        }
                    }
                }
            }
        }
    }

    whiteTurn = old;
    return false;
}

bool isCheckmate(bool whiteChecked)
{
    if (isInCheck(whiteChecked) && !hasAnyLegalMove(whiteChecked))
        return true;
    return false;
}
bool isStalemate(bool whiteChecked)
{
    // Stalemate>> NOT in check, but dont have any legal moves.
    if (!isInCheck(whiteChecked) && !hasAnyLegalMove(whiteChecked))
        return true;
    return false;
}
bool kingExists(bool white) 
{
    char kc;
    if (white)
    {
        kc = 'K';
    }
    else
    {
        kc = 'k';
    }
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            if (boardLogic[r][c] == kc)
            {
                return true;
            }
        }
    }
    return false;
}


// =====================================================
//     SFML BOARD HANDLING
// =====================================================

int boardArr[8][8];

int convert(char Piece) //+for Upper(white Piece) -for Lower(black Piece)
{
    //when we are going to perform move it is easy to campare integers rather than characters so....
    switch (Piece)
    {
        //1 for white Pawn & so on
    case 'P': return 1;
    case 'R': return 2;
    case 'N': return 3;
    case 'B': return 4;
    case 'Q': return 5;
    case 'K': return 6;
        //-1 for Black Pawn & so on
    case 'p': return -1;
    case 'r': return -2;
    case 'n': return -3;
    case 'b': return -4;
    case 'q': return -5;
    case 'k': return -6;
    }
    return 0; //for spaces it return 0
}

char backConvert(int id)
{
    //when board update after move it assign the charater to the pieces (texture requires character not digit as our image name is Character)
    switch (id)
    {
    case 1: return 'P';
    case 2: return 'R';
    case 3: return 'N';
    case 4: return 'B';
    case 5: return 'Q';
    case 6: return 'K';

    case -1: return 'p';
    case -2: return 'r';
    case -3: return 'n';
    case -4: return 'b';
    case -5: return 'q';
    case -6: return 'k';
    }
    return ' '; //for 0 it returns spaces
}

void updateboard()// It actually convert the board into Digital-One
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            boardArr[r][c] = convert(boardLogic[r][c]);
        }
    }
}

// =====================================================
// DRAWING
// =====================================================

float tileW, tileH, offX, offY;

void drawBoard(RenderWindow& win, RectangleShape& box, int highlightRow, int highlightCol)
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            box.setSize(Vector2f(tileW, tileH));

            if (r == highlightRow && c == highlightCol)// highlightRow is function pass same row and same col 
            {
                box.setFillColor(Color::Red);
            }
            else if ((r + c) % 2 == 0)
            {
                box.setFillColor(Color::White); //box is a variable and setfillcolor is used to assign colors
            }
            else
            {
                box.setFillColor(Color(100, 160, 100));
            }

            box.setPosition(c * tileW + offX, r * tileH + offY); //set position is used to define position to box
            win.draw(box);
        }
    }
}

void drawPieces(RenderWindow& win, Texture W[], Texture B[]) {
    Sprite s;                         //Sprite fuction is used to load the pic on the screen which texture contains
    s.setScale(0.75f, 0.75f);         // setscale is a function of SFML it set the position of pics into the box

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            int id = boardArr[r][c];//boardArr contains the numeric chess board
            if (id == 0)
            {
                continue;
            }

            if (id > 0)
            {
                s.setTexture(W[id - 1]); //[id-1] because id start from 1 to 8 for White pieces but our array 0 to 7 so...
            }
            else
            {
                s.setTexture(B[-id - 1]); //[-id-1] because id start from -1 to -8 for black pieces but our array 0 to 7 so...
            }

            s.setPosition(c * tileW + offX, r * tileH + offY);
            win.draw(s);
        }
    }
}

char showPromotionMenu(RenderWindow& window, bool white, Texture W[], Texture B[])
{
    RectangleShape menuBG(Vector2f(tileW * 4, tileH));
    menuBG.setFillColor(Color(200, 200, 200));
    menuBG.setPosition((window.getSize().x - tileW * 4) / 2, (window.getSize().y - tileH) / 2);

    Sprite sprites[4];
    int startX = menuBG.getPosition().x;
    for (int i = 0; i < 4; i++)
    {
        sprites[i].setScale(0.75f, 0.75f);
        sprites[i].setPosition(startX + i * tileW, menuBG.getPosition().y);
        if (white)
        {
            if (i == 0) sprites[i].setTexture(W[1]); // Rook
            if (i == 1) sprites[i].setTexture(W[2]); // Knight
            if (i == 2) sprites[i].setTexture(W[3]); // Bishop
            if (i == 3) sprites[i].setTexture(W[4]); // Queen
        }
        else
        {
            if (i == 0) sprites[i].setTexture(B[1]);
            if (i == 1) sprites[i].setTexture(B[2]);
            if (i == 2) sprites[i].setTexture(B[3]);
            if (i == 3) sprites[i].setTexture(B[4]);
        }
    }

    while (true)
    {
        Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
            {
                window.close();
            }
            if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left)
            {
                int mx = ev.mouseButton.x;
                int idx = (mx - startX) / tileW;
                if (idx >= 0 && idx < 4)
                {
                    if (white) return (idx == 0 ? 'R' : idx == 1 ? 'N' : idx == 2 ? 'B' : 'Q');
                    else return (idx == 0 ? 'r' : idx == 1 ? 'n' : idx == 2 ? 'b' : 'q');
                }
            }
        }

        window.clear();
        RectangleShape box(Vector2f(tileW, tileH));
        drawBoard(window, box, -1, -1);

        drawPieces(window, W, B);
        window.draw(menuBG);
        for (int i = 0; i < 4; i++) window.draw(sprites[i]);
        window.display();
    }
}

char whiteCaptured[16];   // stores captured white pieces
char blackCaptured[16];   // stores captured black pieces
int whiteCapCount = 0;    // number of captured white pieces
int blackCapCount = 0;    // number of captured white pieces


void drawCaptured(RenderWindow& win, Texture W[], Texture B[])
{
    Sprite s;
    s.setScale(0.5f, 0.5f);
    float xWhite = offX + 8 * tileW + 10; // Right of board
    float xBlack = offX - tileW;           // Left of board
    float yStart = offY;

    // Draw light background for captured pieces
    RectangleShape bgWhite(Vector2f(tileW, tileH * 8));
    bgWhite.setFillColor(Color(200, 200, 200)); // Light grey
    bgWhite.setPosition(xWhite, yStart);
    win.draw(bgWhite);

    RectangleShape bgBlack(Vector2f(tileW, tileH * 8));
    bgBlack.setFillColor(Color(200, 200, 200)); // Light grey
    bgBlack.setPosition(xBlack, yStart);
    win.draw(bgBlack);

    // White captured pieces (shown on right)
    for (int i = 0; i < whiteCapCount; i++)
    {
        char c = whiteCaptured[i];
        int idx = convert(c) - 1;
        if (idx >= 0 && idx < 6)
        {
            s.setTexture(W[idx]);
            s.setPosition(xWhite, yStart + i * (tileH / 2));
            win.draw(s);
        }
    }

    // Black captured pieces (shown on left)
    for (int i = 0; i < blackCapCount; i++)
    {
        char c = blackCaptured[i];
        int idx = -convert(c) - 1;
        if (idx >= 0 && idx < 6)
        {
            s.setTexture(B[idx]);
            s.setPosition(xBlack, yStart + i * (tileH / 2));
            win.draw(s);
        }
    }
}


// =====================================================
// DRAG & DROP VARIABLES
// =====================================================

bool dragging = false;
Sprite dragSprite;
int dragR, dragC, highlightRow, highlightCol;
float dragOffsetX, dragOffsetY;
int main()
{
    initializeBoardLogic();//load the Character board
    updateboard(); // updateboard converts Character board into numeric board and store it BoardArr

    VideoMode desk = VideoMode::getDesktopMode();
    RenderWindow window(desk, "Chess (Have Fun!!) ", Style::Default);

    // Load sound files
    if (!moveBuffer.loadFromFile("audio/move.wav"))
    {
        cout << "Move sound missing! ";
    }
    if (!startBuffer.loadFromFile("audio/Start.wav"))
    {
        cout << "Starting sound missing!";
    }
    if (!completingBuffer.loadFromFile("audio/Completing.wav"))
    {
        cout << "Completing missing! ";
    }

    // Assign buffers to sound objects
    moveSound.setBuffer(moveBuffer);
    startSound.setBuffer(startBuffer);
    completingSound.setBuffer(completingBuffer);

    startSound.play();
    Texture texW[6], texB[6];

    texW[0].loadFromFile("pieces/white-pawn.png");
    texW[1].loadFromFile("pieces/white-rook.png");
    texW[2].loadFromFile("pieces/white-knight.png");
    texW[3].loadFromFile("pieces/white-bishop.png");
    texW[4].loadFromFile("pieces/white-queen.png");
    texW[5].loadFromFile("pieces/white-king.png");

    texB[0].loadFromFile("pieces/black-pawn.png");
    texB[1].loadFromFile("pieces/black-rook.png");
    texB[2].loadFromFile("pieces/black-knight.png");
    texB[3].loadFromFile("pieces/black-bishop.png");
    texB[4].loadFromFile("pieces/black-queen.png");
    texB[5].loadFromFile("pieces/black-king.png");


    RectangleShape box;
    Event ev;

    while (window.isOpen())
    {


        float size = min(window.getSize().x, window.getSize().y);
        tileW = tileH = size / 8.0f;
        offX = (window.getSize().x - size) / 2;
        offY = (window.getSize().y - size) / 2;

        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
            {
                completingSound.play();
                sleep(seconds(1)); // give time to play
                window.close();
            }



            if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left) //mouse event for dragging
            {

                int mx = ev.mouseButton.x;
                int my = ev.mouseButton.y;

                int col = (mx - offX) / tileW;
                int row = (my - offY) / tileH;
                cout << "From Square: " << char('A' + col) << 8 - row << " (Row " << (row + 1) << ", Col " << (col + 1) << ")" << endl;

                if (isInsideBoard(row, col) && boardArr[row][col] != 0)
                {
                    dragging = true;
                    dragR = row;
                    dragC = col;

                    int id = boardArr[row][col];
                    dragSprite.setScale(0.75f, 0.75f);

                    if (id > 0)
                    {
                        dragSprite.setTexture(texW[id - 1]);
                    }
                    else
                    {
                        dragSprite.setTexture(texB[-id - 1]);
                    }

                    dragOffsetX = mx - (col * tileW + offX);
                    dragOffsetY = my - (row * tileH + offY);
                }
            }

            // MOUSE UP
            if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Left)
            {


                if (dragging)
                {
                    dragging = false;

                    int mx = ev.mouseButton.x;
                    int my = ev.mouseButton.y;

                    int col = (mx - offX) / tileW;
                    int row = (my - offY) / tileH;
                    cout << "To Square: " << char('A' + col) << 8 - row << " (Row " << (row + 1) << ", Col " << (col + 1) << ")" << endl;
                    if (isInsideBoard(row, col))
                    {
                        if (isValidMove(dragR, dragC, row, col))
                        {
                            // simulate move first to ensure it does not leave own king in check
                            char backup = boardLogic[row][col];
                            char self = boardLogic[dragR][dragC];

                            if (boardLogic[row][col] != ' ') // There is a piece at destination
                            {
                                char captured = boardLogic[row][col];
                                if (isupper(captured))
                                    whiteCaptured[whiteCapCount++] = captured;
                                else
                                    blackCaptured[blackCapCount++] = captured;
                            }

                            boardLogic[row][col] = self;
                            boardLogic[dragR][dragC] = ' ';

                            bool leavesInCheck = isInCheck(whiteTurn);

                            if (leavesInCheck)
                            {
                                // illegal move: leaves king in (or still in) check
                                // undo and reject
                                boardLogic[dragR][dragC] = self;
                                boardLogic[row][col] = backup;

                                highlightRow = row;
                                highlightCol = col;
                                continue; // skip rest
                            }
                            if ((boardLogic[row][col] == 'P' && row == 0) || (boardLogic[row][col] == 'p' && row == 7))
                            {
                                bool isWhite = boardLogic[row][col] == 'P';
                                boardLogic[row][col] = showPromotionMenu(window, isWhite, texW, texB);
                            }

                            updateboard();
                            moveSound.play();

                            // move is legal, already applied
                            updateboard();
                            moveSound.play();

                            // if opponent king no longer exists -> immediate win
                            if (!kingExists(!whiteTurn))
                            {
                                if (whiteTurn)
                                    cout << "White wins (king captured)!" << endl;
                                else
                                    cout << "Black wins (king captured)!" << endl;

                                completingSound.play();
                                sleep(seconds(2));
                                window.close();
                                break;
                            }

                            if (isCheckmate(!whiteTurn)) // The player who just moved could have checkmated the opponent
                            {
                                if (whiteTurn)
                                {
                                    cout << "White wins by Checkmate!" << endl;
                                }
                                else
                                {
                                    cout << "Black wins by Checkmate!" << endl;
                                }
                                completingSound.play();
                                sleep(seconds(2));
                                window.close();
                            }
                            else if (isStalemate(!whiteTurn))
                            {
                                cout << "Stalemate! Draw!" << endl;
                                completingSound.play();
                                sleep(seconds(2));
                                window.close();
                            }


                            whiteTurn = !whiteTurn;
                            highlightRow = -1;
                            highlightCol = -1;

                        }
                        else
                        {

                            highlightRow = row;
                            highlightCol = col;

                        }
                    }
                }
            }
        }

        window.clear();
        drawBoard(window, box, highlightRow, highlightCol);
        drawPieces(window, texW, texB);
        drawCaptured(window, texW, texB);

        if (dragging)
        {
            Vector2i mp = Mouse::getPosition(window);
            dragSprite.setPosition(mp.x - dragOffsetX, mp.y - dragOffsetY);
            window.draw(dragSprite);
        }

        window.display();
    }

}
