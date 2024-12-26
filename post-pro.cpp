#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <Windows.h>

int main() {
    // Ouvre une bo�te de dialogue pour s�lectionner un fichier
    OPENFILENAMEA ofn = {};
    char szFile[260] = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = "Documents\\";
    ofn.lpstrTitle = "S�lectionner un fichier";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn) == TRUE) {
        // Lit le contenu du fichier
        std::ifstream file(ofn.lpstrFile);
        std::string line, content;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        file.close();

        // Suppression des '0' entre 'T' et le num�ro d'outil
        std::regex pattern(R"(T0+([1-9]))");
        content = std::regex_replace(content, pattern, R"(T$1)");

        // Recherche des "M2" � supprimer
        std::string delimiter = "M2";
        size_t pos = 0;
        while ((pos = content.find(delimiter, pos)) != std::string::npos) {
            content.erase(pos, delimiter.length());
        }

        // Recherche de la premi�re occurrence de "M30"
        pos = content.find("M30");
        if (pos != std::string::npos) {
            // Supprime les lignes � remplacer et toutes les autres occurrences de "M30"
            std::string::size_type start_pos = 0;
            while ((pos = content.find("M30", start_pos)) != std::string::npos) {
                content.erase(pos, content.find('\n', pos) - pos + 1);
            }
        }

        // Enregistre les modifications dans un nouveau fichier
        ofn.lpstrFile = NULL;
        ofn.lpstrFileTitle = NULL;
        ofn.lpstrTitle = "Enregistrer sous";
        ofn.lpstrDefExt = "txt";
        ofn.Flags = OFN_OVERWRITEPROMPT;
        if (GetSaveFileNameA(&ofn) == TRUE) {
            std::ofstream new_file(ofn.lpstrFile);
            new_file << content;
            new_file.close();

            // Demande si l'utilisateur veut ouvrir le nouveau fichier g�n�r�
            int answer = MessageBoxA(NULL, "Voulez-vous ouvrir le nouveau fichier g�n�r� ?", "Ouvrir le fichier ?", MB_YESNO | MB_ICONQUESTION);
            if (answer == IDYES) {
                ShellExecuteA(NULL, "open", ofn.lpstrFile, NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }

    return 0;
}