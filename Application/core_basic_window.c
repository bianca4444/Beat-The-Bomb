#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.c"
#include <raylib.h>
#include <time.h>
#define SKYBLUE    (Color){ 102, 191, 255, 255 }


// Definirea dimensiunilor ferestrei
#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

void WrapText(const char* text, float maxWidth, float x, float y, int fontSize, Color color) {
    // Create a buffer to hold the current line
    char buffer[MAX_LINE_LENGTH];
    char wordBuffer[MAX_LINE_LENGTH];
    int bufferIndex = 0;

    // Split the text into words and accumulate their width
    const char* ptr = text;
    float currentWidth = 0.0f;
    float spaceWidth = MeasureText(" ", fontSize);

    while (*ptr != '\0') {
        // Find the next word
        int wordLength = 0;
        while (*ptr != ' ' && *ptr != '\0' && *ptr != '\n') {
            wordBuffer[wordLength++] = *ptr++;
        }

        // Null-terminate the word
        wordBuffer[wordLength] = '\0';

        // Measure the word's width
        float wordWidth = MeasureText(wordBuffer, fontSize);

        // Check if adding the word would exceed the maxWidth
        if (currentWidth + wordWidth > maxWidth) {
            // Terminate the current line buffer
            buffer[bufferIndex] = '\0';

            // Draw the line
            DrawText(buffer, x, y, fontSize, color);

            // Move to the next line
            y += fontSize + 5; // Adjust line height as needed

            // Reset the line buffer
            bufferIndex = 0;
            currentWidth = 0.0f;
        }

        // Add the word to the current line buffer
        strncpy(&buffer[bufferIndex], wordBuffer, wordLength);
        bufferIndex += wordLength;
        currentWidth += wordWidth;

        // Add a space if necessary
        if (*ptr == ' ') {
            buffer[bufferIndex++] = ' ';
            currentWidth += spaceWidth;
            ptr++;
        }
    }

    // Draw any remaining text in the buffer
    buffer[bufferIndex] = '\0';
    DrawText(buffer, x, y, fontSize, color);
}

int main(void) {
    FILE *file = fopen("input1.csv", "r");
    if (file == NULL) {
        fprintf(stderr, "Nu s-a putut deschide fișierul.\n");
        exit(EXIT_FAILURE);
    }

    int src, dest;
    int num_vertices = 0;
    int num_edges = 0;

    char line[MAX_LINE_LENGTH];
    char *token;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        token = strtok(line, ",");
        src = atoi(token);

        token = strtok(NULL, ",");
        dest = atoi(token);

        if (src > num_vertices) num_vertices = src;
        if (dest > num_vertices) num_vertices = dest;
        num_edges++;
    }

    num_vertices++;
    printf("%d și %d\n", num_edges, num_vertices);

    struct Edge edges[num_edges];
    int index = 0;

    rewind(file);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        token = strtok(line, ",");
        src = atoi(token);

        token = strtok(NULL, ",");
        dest = atoi(token);

        edges[index].src = src;
        edges[index].dest = dest;
        index++;
    }

    fclose(file);
    struct Graph *graph = createGraph(edges, num_edges, num_vertices);

    printGraph(graph, num_vertices);

    // Initialization
    const int screenWidth = 1900;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Beat The Bomb");
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitAudioDevice();
    Sound sound = LoadSound("explosion-42132.mp3");
    SetSoundVolume(sound, 0.1);

    Image bomb = LoadImage("bomb.png");
    Texture2D texture = LoadTextureFromImage(bomb);
    UnloadImage(bomb);

    Image sticker = LoadImage("sticker.png");
    int newWidth = sticker.width * 2;
    int newHeight = sticker.height * 2;
    ImageResize(&sticker, newWidth, newHeight);
    Texture2D texture2 = LoadTextureFromImage(sticker);

    UnloadImage(sticker);

    SetTargetFPS(60);


    int math;
    // Variables
    float rectWidth = 300, rectHeight = 300;
    float rectPosX = (screenWidth - rectWidth) / 2;
    float rectPosY = (screenHeight - rectHeight - 50) / 2;
    char mathExpression[10000];
    char mathExpression2[10000];
    int category_index = 0;
    char mathExpressionOld[10000];

    const float clickDuration = 1.0f;

    char correctAnswer;
    char userAnswer;
    bool showCorrectMessage = false;
    float correctStartTime = 0.0f;

    char userInput[10] = ""; // Array to store user input
    int inputIndex = 0; // Current index for user input
    bool inputSubmitted = false; // Flag to track if input has been submitted
    bool answerCorrect = false; // Flag to track if the answer is correct

    bool trueClicked = false;
    bool falseClicked = false;
    float trueClickTimer = 0.0f;
    float falseClickTimer = 0.0f;
    int save;
    // Define button positions and sizes
    Rectangle trueButton = {screenWidth/2-300, screenHeight / 2 +250, 150, 100};
    Rectangle falseButton = {screenWidth/2+150, screenHeight / 2 +250, 150, 100};
    // Define colors for the buttons
    Color trueColor = LIGHTGRAY;
    Color falseColor = LIGHTGRAY;

    // Loading bar
    float barWidth = 600;
    float barHeight = 20;
    float barPosX = (screenWidth - barWidth) / 2;
    float barPosY = screenHeight - barHeight - 50;
    float duration = 20.0f; // Timer duration in seconds
    float startTime = GetTime();
    float elapsedTime = 0.0f;
    bool stopTimer = false; // Flag to stop the timer
    bool gameStarted = false; // Flag to track if the game has started
    bool gameOver = false;
    int corecte=0;
    char c_score[10];
    int incorecte=0;
    char i_score[10];
    bool playsound = false;
    struct node *ptr = graph->head[0];
    struct node_secund *ptrs = ptr->next_secundar;
    // Main game loop


    while (!WindowShouldClose()) {

        if (!gameStarted) {
            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true; // Start the game when space is pressed
                gameOver = false;
                stopTimer = false; // Reset stopTimer flag
                answerCorrect = false;
                math = 0;
                sprintf(mathExpressionOld, "%s", mathExpression);
                sprintf(mathExpression2, "%s", ptr->continut_nod_graf_principal);
                ptr = ptr->next;
                sprintf(mathExpression, "%s", ptrs->continut_nod_graf_secundar);
                correctAnswer = ptrs->answer[0];
                ptrs = ptrs->next;
                // Reset the timer
                startTime = GetTime();
                elapsedTime = 0.0f;
                corecte=0;
                incorecte=0;
                category_index=0;
            }
        } else {

            // Update timer and check for game over condition
            if (!stopTimer) {
                float currentTime = GetTime();
                elapsedTime = currentTime - startTime;
                float progress = elapsedTime / duration;

                if (elapsedTime >= duration || category_index == 4) {
                    stopTimer = true; // End the timer when the duration is reached
                }

                if (!inputSubmitted) {
                    if (CheckCollisionPointRec(GetMousePosition(), trueButton)) {
                        trueColor = GRAY; // Change color to indicate hover
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            // True button clicked
                            trueColor = RED; // Set color to indicate click
                            trueClicked = true;
                            trueClickTimer = clickDuration;
                            inputSubmitted = true;
                            math++;
                            userAnswer = 'T';
                            answerCorrect = (userAnswer == correctAnswer);
                            // You can add your desired functionality here
                        }
                    } else {
                        trueColor = LIGHTGRAY; // Reset color when not hovered
                    }

                    // Check for False button click
                    if (CheckCollisionPointRec(GetMousePosition(), falseButton)) {
                        falseColor = GRAY; // Change color to indicate hover
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            // False button clicked
                            falseColor = RED; // Set color to indicate click
                            falseClicked = true;
                            falseClickTimer = clickDuration;
                            inputSubmitted = true;
                            math++;
                            userAnswer = 'F';
                            answerCorrect = (userAnswer == correctAnswer);
                            // You can add your desired functionality here
                        }
                    } else {
                        falseColor = LIGHTGRAY; // Reset color when not hovered
                    }
                    if(inputSubmitted) {
                        if (answerCorrect) {
                            // Recover 5 seconds from the timer if the answer is correct
                            if (elapsedTime > 5) {
                                elapsedTime -= 5.0f;
                                startTime = currentTime - elapsedTime;
                            } else {
                                elapsedTime = 0;
                                startTime = currentTime - elapsedTime;
                            }
                            corecte++;
                            sprintf(c_score, "%d",corecte);
                        }
                        else {
                            incorecte++;
                            sprintf(i_score, "%d",incorecte);
                        }
                        showCorrectMessage = true;
                        correctStartTime = GetTime();

                        if (ptrs != NULL) {
                            sprintf(mathExpressionOld, "%s", mathExpression);
                            sprintf(mathExpression, "%s", ptrs->continut_nod_graf_secundar);
                            correctAnswer = ptrs->answer[0];
                            ptrs = ptrs->next;
                        } else {
                            category_index++;
                            if (category_index != 4) {
                                ptr = graph->head[category_index];
                                ptrs = ptr->next_secundar;
                                sprintf(mathExpressionOld, "%s", mathExpression);
                                sprintf(mathExpression2, "%s", ptr->continut_nod_graf_principal);
                                sprintf(mathExpression, "%s", ptrs->continut_nod_graf_secundar);
                                correctAnswer = ptrs->answer[0];
                                ptrs = ptrs->next;
                            }

                        }
                        inputSubmitted = false;
                    }
                }

// Update the timers and reset flags if necessary
                if (trueClicked) {
                    trueClickTimer -= GetFrameTime();
                    if (trueClickTimer <= 0.0f) {
                        trueClicked = false;
                    }
                }

                if (falseClicked) {
                    falseClickTimer -= GetFrameTime();
                    if (falseClickTimer <= 0.0f) {
                        falseClicked = false;
                    }
                }

            }
        }


        // Draw
        BeginDrawing();
        ClearBackground(BLACK);


        if (!gameStarted && !gameOver) {
            // Display message to press Space to start the game
            DrawText("Welcome to Beat The Bomb", (screenWidth - MeasureText("Welcome to Beat The Bomb", 40)) / 2, screenHeight / 2 - 100,40, RAYWHITE);

            DrawText("Press SPACE to start", (screenWidth - MeasureText("Press SPACE to start", 40)) / 2, screenHeight / 2,
                     40, RAYWHITE);

            DrawText("EU Programme Erasmus+", screenWidth / 35, screenHeight / 20,40, SKYBLUE);

            DrawText("Project title: Gender, Digitalization, Green: Ensuring a Sustainable Future for all in Europe", screenWidth / 35, screenHeight / 10,35, SKYBLUE);

            DrawText("Project N°: 2023-1-RO01-KA220-HED-000154433", screenWidth / 35, screenHeight / 7,40, SKYBLUE);

            DrawText("( Module 5 )", screenWidth / 35, screenHeight / 5,35, SKYBLUE);


            DrawText("Students:", screenWidth / 35, screenHeight / 2 + 100,40, SKYBLUE);

            DrawText("Dumitrescu Bianca-Alexandra ( lider ) - email biancadumitrescu25@gmail.com", screenWidth / 35, screenHeight / 2 + 135,35, SKYBLUE );

            DrawText("Joita Fabian Gabriel - email joitafabi@gmail.com", screenWidth / 35, screenHeight / 2 + 170,35, SKYBLUE);

            DrawText("Jugulete George Marius - email georgejugulete915@gmail.com", screenWidth / 35, screenHeight / 2 + 205 ,35, SKYBLUE);

            DrawText("Sandu Andreea Diana - email diana.sandu008@gmail.com", screenWidth / 35, screenHeight / 2 + 240,35, SKYBLUE);

            DrawText("Manescu Daria-Ioana - email dariamanescuioana8@gmail.com", screenWidth / 35, screenHeight / 2 + 275,35, SKYBLUE);

            DrawText("Teachers:", screenWidth / 35, screenHeight / 2 + 320,40, SKYBLUE);

            DrawText("Prof. M. Caramihai & D. Chis", screenWidth / 35, screenHeight / 2 + 360,35, SKYBLUE);

        } else {

            ClearBackground(RAYWHITE);
            DrawRectangle(0, 0, screenWidth / 4, screenHeight , BLACK);
            DrawTexture(texture, screenWidth/7 - texture.width/2, screenHeight/4 + texture.height/2 - 40, WHITE);

            WrapText(mathExpression2, 1300, 500, 250, 30, BLACK);
            // Draw the True button

            DrawRectangleRec(trueButton, trueClicked ? ORANGE : YELLOW);
            DrawText("True", (int) (trueButton.x + 50), (int) (trueButton.y + 40), 20, BLACK);

            // Draw the False button
            DrawRectangleRec(falseButton, falseClicked ? ORANGE : YELLOW);
            DrawText("False", (int) (falseButton.x + 50), (int) (falseButton.y + 40), 20, BLACK);


            // Draw math expression and user input
            if (!showCorrectMessage) {
                WrapText(mathExpression, 1300, 500, 365, 30, BLACK);

            }
            // Display feedback if the input has been submitted


            if (showCorrectMessage) {
                float currentCorrectTime = GetTime();
                float correctElapsedTime = currentCorrectTime - correctStartTime;

                if (correctElapsedTime < 1.0f) {
                    // Continue showing the math expression and "Correct" message
                    WrapText(mathExpressionOld, 1300, 500, 365, 30, BLACK);

                    if (math != 0 && !answerCorrect)
                        DrawText("Incorrect!", (screenWidth - MeasureText("Incorrect!", 20)) / 2,
                                 (screenHeight - 100) / 2 - 100 +200, 30, RED);

                    if(answerCorrect)
                        DrawText("Correct!", (screenWidth - MeasureText("Correct!", 20)) / 2,
                                 (screenHeight - 100) / 2 - 100 + 200, 30, GREEN);
                } else {
                    // Stop showing the correct message and move on
                    showCorrectMessage = false;
                }
            }

            DrawText("Incorrects:", 80, 150, 50, RED);
            if(incorecte!=0)
                DrawText(i_score, 390, 150, 50, RED);
            else
                DrawText("0", 390, 150, 50, RED);

            DrawText("Corrects:", 100, 220, 50, GREEN);
            if(corecte!=0)
                DrawText(c_score, 360, 220, 50, GREEN);
            else
                DrawText("0", 360, 220, 50, GREEN);


            // Draw loading bar if the game is not over
            if (!stopTimer) {
                float progress = elapsedTime / duration;
                float filledWidth = barWidth * progress;

                DrawRectangle(barPosX, barPosY, barWidth, barHeight, LIGHTGRAY);
                DrawRectangle(barPosX, barPosY, filledWidth, barHeight, RED);

                char counterText[20];
                sprintf(counterText, "%.2f s", elapsedTime);
                int counterTextWidth = MeasureText(counterText, 20);
                DrawText(counterText, barPosX + (barWidth - counterTextWidth) / 2, barPosY + barHeight + 10, 20, BLACK);
            } else if (stopTimer == 1 && category_index <= 3) {
                // Display "Game Over" when the timer stops
                if (!IsSoundPlaying(sound) && playsound == false) {
                    PlaySound(sound); // Play the sound again if it's finished
                    playsound = true;
                }
                if (stopTimer && (!inputSubmitted || !answerCorrect)) {

                    DrawRectangle(0, 0, screenWidth , screenHeight , RAYWHITE);
                    DrawTexture(texture2, screenWidth/2 - 520, screenHeight/2 - 500, WHITE);

                    DrawText("Game Over", (screenWidth - MeasureText("Game Over", 40)) / 2, screenHeight / 2, 40, RED);
                    // sound


                    DrawText("Press SPACE to restart", (screenWidth - MeasureText("Press SPACE to restart", 20)) / 2,
                             screenHeight / 2 + 150, 20, BLACK);
                    playsound = false;
                    gameStarted = false;
                    gameOver = true;
                    ptr = graph->head[0];
                    ptrs = ptr->next_secundar;
                }
            } else if (category_index == 4 && stopTimer == 1) {

                DrawText("You WIN!", (screenWidth - MeasureText("Game Over", 40)) / 2, screenHeight / 2, 40, GREEN);
                gameStarted = false;
                gameOver = true;
                DrawText("Press SPACE to restart", (screenWidth - MeasureText("Press SPACE to restart", 20)) / 2,
                         screenHeight / 2 + 150, 20, BLACK);
                ptr = graph->head[0];
                ptrs = ptr->next_secundar;


            }
        }

        EndDrawing();
    }

    // Cleanup and close window
    CloseAudioDevice();
    UnloadSound(sound);
    CloseWindow();



    return 0;
}