#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include <vector>
#include <string>

namespace ClassGame {
    //
    // Console system
    //
    enum class LogLevel {
        Info,
        Warn,
        Error
    };
    
    struct LogEntry {
        std::string message;
        LogLevel level;
    };
    
    struct Console {
        std::vector<LogEntry> logs;
        char inputBuf[256] = "";
        bool autoScroll = true;
        
        void AddLog(const std::string& message, LogLevel level = LogLevel::Info) {
            logs.push_back({message, level});
        }
        
        void Clear() {
            logs.clear();
        }
        
        const char* GetLevelPrefix(LogLevel level) {
            switch (level) {
                case LogLevel::Info:  return "[Info] ";
                case LogLevel::Warn:  return "[Warn] ";
                case LogLevel::Error: return "[Error] ";
                default: return "";
            }
        }
        
        void Draw() {
            ImGui::Begin("Console");
            
            if (ImGui::Button("Clear")) {
                Clear();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            
            ImGui::Separator();
            
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
            
            for (const auto& log : logs) {
                ImVec4 color;
                switch (log.level) {
                    case LogLevel::Info:
                        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
                        break;
                    case LogLevel::Warn:
                        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                        break;
                    case LogLevel::Error:
                        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                        break;
                }
                std::string fullMessage = GetLevelPrefix(log.level) + log.message;
                ImGui::TextColored(color, "%s", fullMessage.c_str());
            }
            
            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
                
            ImGui::EndChild();
            
            ImGui::End();
        }
    };
    
    //
    // Global variables
    //
    static Console g_Console;
    TicTacToe *game = nullptr;
    bool gameOver = false;
    int gameWinner = -1;
    bool aiEnabled = false;

    //
    // Game starting point
    //
    void GameStartUp() 
    {
        game = new TicTacToe();
        game->setUpBoard();
        
        g_Console.AddLog("Player 1 (O) vs Player 2 (X)", LogLevel::Info);
        g_Console.AddLog("Player 1's turn", LogLevel::Info);
    }

    //
    // Game render loop
    //
    void RenderGame() 
    {
        ImGui::DockSpaceOverViewport();

        if (!game) return;
        if (!game->getCurrentPlayer()) return;
        
        ImGui::Begin("Settings");
        ImGui::Text("Current Player: %d (%s)", 
                    game->getCurrentPlayer()->playerNumber() + 1,
                    game->getCurrentPlayer()->playerNumber() == 0 ? "O" : "X");
        ImGui::Text("Current Board State: %s", game->stateString().c_str());

        // AI Toggle
        if (ImGui::Checkbox("Enable AI (Player 2)", &aiEnabled)) {
            if (aiEnabled) {
                g_Console.AddLog("AI enabled for Player 2 (X)", LogLevel::Info);
            } else {
                g_Console.AddLog("AI disabled", LogLevel::Info);
            }
        }

        if (gameOver) {
            ImGui::Text("Game Over!");
            if (gameWinner == -1) {
                ImGui::Text("Draw!");
            } else {
                ImGui::Text("Winner: Player %d (%s)", 
                           gameWinner + 1,
                           gameWinner == 0 ? "O" : "X");
            }
            if (ImGui::Button("Reset Game")) {
                game->stopGame();
                game->setUpBoard();
                gameOver = false;
                gameWinner = -1;
                g_Console.AddLog("Game reset", LogLevel::Info);
            }
        }
        ImGui::End();

        ImGui::Begin("GameWindow");
        game->drawFrame();
        ImGui::End();
        
        // Draw console
        g_Console.Draw();

        //Check if it's AI turn and make a move
        if (!gameOver && aiEnabled && game->getCurrentPlayer()->playerNumber() == 1) {
            game->updateAI();
        }
    }

    //
    // End turn is called by the game code at the end of each turn
    //
    void EndOfTurn() 
    {
        Player *winner = game->checkForWinner();
        if (winner)
        {
            gameOver = true;
            gameWinner = winner->playerNumber();
            
            std::string winnerName = (gameWinner == 0) ? "O" : "X";
            g_Console.AddLog("=== GAME OVER ===", LogLevel::Warn);
            g_Console.AddLog("Winner: Player " + std::to_string(gameWinner + 1) + 
                           " (" + winnerName + ")", LogLevel::Info);
            g_Console.AddLog("Congratulations!", LogLevel::Info);
            return;
        }
        
        if (game->checkForDraw()) {
            gameOver = true;
            gameWinner = -1;
            
            g_Console.AddLog("=== GAME OVER ===", LogLevel::Warn);
            g_Console.AddLog("It's a Draw! No winner.", LogLevel::Info);
            return;
        }
        
        // Log next player's turn
        int nextPlayer = game->getCurrentPlayer()->playerNumber();
        std::string playerName = (nextPlayer == 0) ? "O" : "X";
        g_Console.AddLog("Player " + std::to_string(nextPlayer + 1) + " (" + playerName + ")'s turn", LogLevel::Info);
    }
}