#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <utility>

struct Position {
	int row;
	int col;

	bool operator == (const Position& other) const {
		return row == other.row && col == other.col;;
	}
};

class Maze {
private:
	int rows = 0;
	int cols = 0;
	std::vector<std::string> grid;
	Position startPos = { -1, -1 };
	Position exitPos = { -1, -1 };

public:
	bool loadFromFile(const std::string& filename) {
		std::ifstream file(filename);

		if (!file.is_open()) {
			std::cerr << "Error: Could not open file " << filename << "\n";
			return false;
		}
		if (!(file >> rows >> cols)) {
			std::cerr << "Error: Invalid header dimensions." << std::endl;
			return false;
		}
        file.ignore(); 

        std::string line;
        for (int r = 0; r < rows; ++r) {
            if (std::getline(file, line)) {
                grid.push_back(line);
                for (int c = 0; c < static_cast<int>(line.size()); ++c) {
                    if (line[c] == 'P') startPos = { r, c };
                    if (line[c] == 'E') exitPos = { r, c };
                }
            }
        }

        file.close();
        return (startPos.row != -1 && exitPos.row != -1);
    }

   
    bool solveWithStack() {
        if (startPos.row == -1 || exitPos.row == -1) {
            std::cerr << "Error: Missing start or exit position.\n";
            return false;
        }

        std::stack<Position> pathStack;
        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

       
        const int dRow[] = { 1, 0, -1, 0 };
        const int dCol[] = { 0, 1, 0, -1 };

        pathStack.push(startPos);
        visited[startPos.row][startPos.col] = true;

        while (!pathStack.empty()) {
            Position current = pathStack.top();

            
            if (current == exitPos) {
                markPathOnGrid(pathStack);
                return true;
            }

            bool moved = false;

            
            for (int i = 0; i < 4; ++i) {
                int nextRow = current.row + dRow[i];
                int nextCol = current.col + dCol[i];

                if (isValidMove(nextRow, nextCol, visited)) {
                    visited[nextRow][nextCol] = true;
                    pathStack.push({ nextRow, nextCol });
                    moved = true;
                    break; 
                }
            }

            
            if (!moved) {
                pathStack.pop();
            }
        }

        return false; 
    }

    void display() const {
        for (const auto& row : grid) {
            std::cout << row << '\n';
        }
    }

private:
    bool isValidMove(int r, int c, const std::vector<std::vector<bool>>& visited) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return false; 
        if (grid[r][c] == '#') return false;                       
        if (visited[r][c]) return false;                            
        return true;
    }

    void markPathOnGrid(std::stack<Position> pathStack) {
        while (!pathStack.empty()) {
            Position pos = pathStack.top();
            pathStack.pop();
           
            if (grid[pos.row][pos.col] != 'P' && grid[pos.row][pos.col] != 'E') {
                grid[pos.row][pos.col] = '*';
            }
        }
    }
};

int main()
{
    Maze maze;

    if (!maze.loadFromFile("maze.txt")) {
        std::cerr << "Failed to load maze from file.\n";
        return 1;
    }

    std::cout << "Original Maze:\n";
    maze.display();

    std::cout << "\n-----------------------------\n";

    if (maze.solveWithStack()) {
        std::cout << "Path found using Stack Traversal (*):\n\n";
        maze.display();
    }
    else {
        std::cout << "No solution exists for this maze.\n";
    }

    return 0;
}
