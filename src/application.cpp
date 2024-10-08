// Elo Maluco
// application.cpp

#include <stdlib.h>
#include <stdio.h>
#include "application.hpp"

///////////////////////////////////////////////////////////////////////
// Application Class (construtor)
Application::Application(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("ELO MALUCO");
    Inicializa();

    processXML("../data/EloMaluco_estadoAtual_teste01.xml");

    insert_object();
}

//---------------------------------------------------------------------
// Destrutor
Application::~Application()
{
}

//---------------------------------------------------------------------
void Application::Inicializa(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // cor da janela
    xf = 50.0f;
    yf = 50.0f;
    win = 250.0f;
    time = 0;
}

//---------------------------------------------------------------------
// void Application::DisplayFunc()
//{
//	glutDisplayFunc(Application::Desenha);
//}

//---------------------------------------------------------------------
// faz o mapeamento das cores no xml para o formato rgb que precisamos
Color Application::mapColor(const string &code)
{
    if (code == "vms" || code == "vmi" || code == "vmm")
        return {1.0f, 0.0f, 0.0f}; // vrmelho
    if (code == "vds" || code == "vdi" || code == "vdm" || code == "vrs" || code == "vri" || code == "vrm")
        return {0.0f, 1.0f, 0.0f}; // Verde
    if (code == "ams" || code == "ami" || code == "amm")
        return {1.0f, 1.0f, 0.0f}; // Amarelo
    if (code == "brs" || code == "bri" || code == "brm")
        return {1.0f, 1.0f, 1.0f}; // branco
    if (code == "vzo")
        return {0.5f, 0.5f, 0.5f}; // cinza
    return {0.0f, 0.0f, 0.0f};     // preto
}

//---------------------------------------------------------------------
// imprimi a matriz das cores no console para conferir se está tudo correto
void Application::printColorMatrix() const
{
    std::cout << "Color Matrix:" << std::endl;
    for (size_t i = 0; i < colorMatrix.size(); ++i)
    {
        std::cout << "Row " << i << ": ";
        for (size_t j = 0; j < colorMatrix[i].size(); ++j)
        {
            const glm::vec3 &color = colorMatrix[i][j];
            std::cout << "(" << color.r << ", " << color.g << ", " << color.b << ") ";
        }
        std::cout << std::endl;
    }
}

//---------------------------------------------------------------------
// Leitura do arquivo xml e processamento dos dados sobre as cores contidos nele. Esses dados sao  inseridoas na matriz de cores
void Application::processXML(const string &filename)
{

    XMLDocument doc;

    // verificação de carregamento do arquivo
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS)
    {
        cerr << "Erro ao carregar o arquivo XML. Verifique o caminho." << endl;
        return;
    }

    // Verifica se o xml contem o elemento raiz
    XMLElement *root = doc.RootElement();
    if (root == nullptr)
    {
        cerr << "O elemento raiz não foi encontrado." << endl;
        return;
    }

    // Verifica se existem os elemntos dentro da raiz
    XMLElement *estadoAtual = root->FirstChildElement("EstadoAtual");
    if (estadoAtual == nullptr)
    {
        cerr << "Nenhum elemento encontrado." << endl;
        return;
    }

    colorMatrix.clear();

    // percorre cada row e col's dentro dela. Armazena seus respctivos codigos (utilizando o mapeamento) dentro da nossa matriz de cores
    for (XMLElement *row = estadoAtual->FirstChildElement("row"); row != nullptr; row = row->NextSiblingElement("row"))
    {
        vector<vec3> colorRow;
        for (XMLElement *col = row->FirstChildElement("col"); col != nullptr; col = col->NextSiblingElement("col"))
        {
            const char *text = col->GetText();
            if (text)
            {
                string code(text);
                Color color = mapColor(code);
                colorRow.push_back(vec3(color.r, color.g, color.b)); // Conversão das cores para vec3
            }
        }
        colorMatrix.push_back(colorRow);
    }
    printColorMatrix();
}

//---------------------------------------------------------------------
void Application::draw()
{
    glEnable(GL_DEPTH_TEST); // arruma a profundidade

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa a janela e o Depth Buffer
    glLoadIdentity();

    glTranslatef(7.5f, 0.0f, 7.5f);

    glRotatef(globalRotation, 0.0f, 1.0f, 0.0f);

    glTranslatef(-7.5f, 0.0f, -7.5f);

    // Desenha os eixos x, y e z
    // glLineWidth(2.0f);
    // glBegin(GL_LINES);

    // glColor3f(1.0f, 0.0f, 0.0f); //vermelho
    // glVertex3f(-100.0f, 0.0f, 0.0f);
    // glVertex3f(100.0f, 0.0f, 0.0f);

    // glColor3f(0.0f, 1.0f, 0.0f); //verde
    // glVertex3f(0.0f, -100.0f, 0.0f);
    // glVertex3f(0.0f, 100.0f, 0.0f);

    // glColor3f(0.0f, 0.0f, 1.0f); //azual
    // glVertex3f(0.0f, 0.0f, -100.0f);
    // glVertex3f(0.0f, 0.0f, 100.0f);

    // glEnd();

    // glColor3f(1.0f, 1.0f, 1.0f);

    // Desenha os cubos
    for (auto obj : list_)
    {
        obj->draw();
    }

    // Desenhado alguns textos na tela
    drawFixedText();

    // mostrando
    glutSwapBuffers();
}
//---------------------------------------------------------------------
// Função para desenhar o texto fixo na tela (Não vai girar junto com o Elo)
void Application::drawFixedText()
{
    // Salva as configurações de projeção e visão
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Define a perspectiva da câmera especifica para o texto
    gluPerspective(55, (GLdouble)view_w / view_h, 1, 500);
    gluLookAt(0.0, 0.0, 150.0, 7.5f, 30.0f, 7.5f, 0.0, 1.0, 0.0);

    // Salva a configuração de modelo e visão
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Define a cor do texto
    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para o texto

    // Desenhando umas instruções aleatorias que vão ser colocadas no menu depois
    drawText(-60.0f, -15.f, "Instrucoes para jogar: ");
    drawText(-60.0f, -22.f, "Setas para esquera e direita movem o Elo Maluco;");
    drawText(-60.0f, -27.f, "Setas para cima e para baixo movem a casa vazia;");
    drawText(-60.0f, -32.f, "F1, F2, F3 e F4 selecionam o cubo;");
    drawText(-60.0f, -37.f, "Botoes do Mouse rotacionam o cubo;");

    // Vericiando qual cubo foi selecionado, para mostrar uma seta informativa 
    if (selectedCubeIndex != -1)
    {
        float textPosX = -15.0f; // Posição no eixo x da seta
        float textPosY = 0.0f;

        // Definindo cada posicão em y de acordo com o cubo selecionado
        if (selectedCubeIndex == 0)
        {
            textPosY = 52.5f;
        }
        else if (selectedCubeIndex == 1)
        {
            textPosY = 37.5f;
        }
        else if (selectedCubeIndex == 2)
        {
            textPosY = 22.5f;
        }
        else
        {
            textPosY = 7.5f;
        }

        // Desenha a seta na posição do cubo selecionado
        drawText(textPosX, textPosY, "->");
    }

    // Restaura as configurações originais
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
//---------------------------------------------------------------------
// Função auxiliar para desenhar texto na tela 
void Application::drawText(float x, float y, const char *message)
{
    glRasterPos2f(x, y); // Define a posição do texto no espaço 2D
    for (const char *c = message; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); // Desenha cada caractere
    }
}
//---------------------------------------------------------------------
void Application::resize(GLsizei w, GLsizei h)
{
    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);
    view_w = w;
    view_h = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // definição da pespectiva da câmera
    gluPerspective(60, (GLdouble)view_w / view_h, 1, 500);
    gluLookAt(0.0, 0.0, 150.0,   // Posição da câmera
              7.5f, 30.0f, 7.5f, // Ponto para onde a câmera olha
              0.0, 1.0, 0.0);    // Vetor "up"

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//---------------------------------------------------------------------
void Application::KeyboardHandle(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'R':
    case 'r': // muda a cor corrente para vermelho
        glColor3f(1.0f, 0.0f, 0.0f);
        break;
    case 'G':
    case 'g': // muda a cor corrente para verde
        glColor3f(0.0f, 1.0f, 0.0f);
        break;
    case 'B':
    case 'b': // muda a cor corrente para azul
        glColor3f(0.0f, 0.0f, 1.0f);
        time++;
        for (list<Objects *>::const_iterator it = list_.begin(); it != list_.end(); ++it)
        {
            (*it)->update(time);
        }
        break;
    case 27: // tecla Esc (encerra o programa)
        exit(0);
        break;
    }
}
//---------------------------------------------------------------------
// Função que gerencia os clicks do mouse
void Application::MouseHandle(int button, int state, int x, int y)
{
    // Se o botão esquerdo do mouse for prescionado ele chama a função de rotação ṕara a esquerda
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
    {
        rotateLeft();
    }
    // Se o botão direito do mouse for prescionado ele chama a função de rotação ṕara a direita
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        rotateRight();
    }
}
//---------------------------------------------------------------------
void Application::SpecialKeyHandle(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        globalRotation -= 5.0f; // Rotaciona 5 graus no sentido anti-horário
        break;
    case GLUT_KEY_RIGHT:
        globalRotation += 5.0f; // Rotaciona 5 graus no sentido horário
        break;
    case GLUT_KEY_UP: // Mover para cima a casa vazia
        moveEmptyHouse(-1);
        break;
    case GLUT_KEY_DOWN: // Mover para baixo a casa vazia
        moveEmptyHouse(1);
        break;
    case GLUT_KEY_F1: // Seleciona o cubo 1
        selectCube(0);
        break;
    case GLUT_KEY_F2: // Seleciona o cubo 2
        selectCube(1);
        break;
    case GLUT_KEY_F3: // Seleciona o cubo 2
        selectCube(2);
        break;
    case GLUT_KEY_F4: // Seleciona o cubo 2
        selectCube(3);
        break;
    }

    glutPostRedisplay(); // Re-desenhar a cena
}

//---------------------------------------------------------------------
void Application::update(int value, void (*func_ptr)(int))
{
    glutPostRedisplay();
    glutTimerFunc(30, func_ptr, time);
}

//---------------------------------------------------------------------
// Conversão de cor, de acordo com o modelo
vec3 colorToVec3(const Color &color)
{
    return glm::vec3(color.r, color.g, color.b);
}

//---------------------------------------------------------------------
bool Application::insert_object()
{
    Cube *cube = new Cube(colorMatrix);
    list_.push_back(cube);
    return true;
}
//---------------------------------------------------------------------

// Encontra a casa vazia e retorna um par de inteiros (linha e coluna dessa casa)
pair<int, int> Application::findEmptyHouse()
{
    for (size_t i = 0; i < colorMatrix.size(); ++i) //Percorre as linhas
    {
        for (size_t j = 0; j < colorMatrix[i].size(); ++j) // Percorre as colunas
        {
            if (colorMatrix[i][j] == vec3(0.5f, 0.5f, 0.5f)) // Se a casa for vazia (cinza)
            { 
                return {static_cast<int>(i), static_cast<int>(j)};
            }
        }
    }
    return {-1, -1}; // Retorna -1, -1 se não encontrar
}
//---------------------------------------------------------------------
// Movimentação da casa vazia
void Application::moveEmptyHouse(int direction)
{
    pair<int, int> emptyHousePosition = findEmptyHouse(); // Primeiro localiza a casa vaxia
    int emptyRow = emptyHousePosition.first;
    int emptyCol = emptyHousePosition.second;

    // Verifica se a casa vazia foi encontrada
    if (emptyRow == -1)
    //Adicionar um possivel tratamento de erro
        return;

    // Calcula a nova linha, para onde o vazio deve ser movido
    int newRow = emptyRow + direction; 

    // Verifica se o novo índice está dentro da matriz
    if (newRow >= 0 && newRow < colorMatrix.size())
    {
        // Troca as cores
        swap(colorMatrix[emptyRow][emptyCol], colorMatrix[newRow][emptyCol]);

        printColorMatrix();

        // Atualiza a matriz de cores de geração dos cubos
        updateCubeColors();

        glutPostRedisplay();
    }
}
//---------------------------------------------------------------------
void Application::selectCube(int index)
{
    selectedCubeIndex = index; // Define o índice do cubo selecionado
}
//---------------------------------------------------------------------
// Rotação para a esquerda
void Application::rotateLeft()
{
    if (selectedCubeIndex != -1) // Verifica se o cubo foi selecionado
    {
        auto &matrix = colorMatrix[selectedCubeIndex]; // Paassa a matriz/linha do cubo selecionado por referencia
        vec3 aux = matrix[0]; // Guarda a primeira cor (pois ela vai virar a ultima devido ao deslocamento)

        for (int i = 0; i < matrix.size() - 1; ++i)
        {
            matrix[i] = matrix[i + 1]; // Desloca para a esquerda todos as cores
        }

        matrix.back() = aux;  // Coloca a primeira cor no final
        cout << "Cubo girado à esquerda." << endl; // Mensagem para conferir no terminal

        printColorMatrix();

        // Atualiza a matriz de cores de todos os cubos
        updateCubeColors();

        glutPostRedisplay();
    }
}
//---------------------------------------------------------------------
// Rotação para a direita
void Application::rotateRight()
{
    if (selectedCubeIndex != -1)
    {
        auto &matrix = colorMatrix[selectedCubeIndex]; // Paassa a matriz/linha do cubo selecionado por referencia
        vec3 aux = matrix.back(); // Guarda a última cor, pois ela vai virar a primeira

        for (int i = matrix.size() - 1; i > 0; --i)
        {
            matrix[i] = matrix[i - 1]; // Desloca para a direita
        }

        matrix[0] = aux; // Coloca a última cor no início
        cout << "Cubo girado à direita." << endl; // Mensagem no terminal

        printColorMatrix();

        // Atualiza a matriz de cores de todos os cubos
        updateCubeColors();

        glutPostRedisplay();
    }
}
//---------------------------------------------------------------------
// Atualização da matriz de cores nas alterações especificas
void Application::updateCubeColors()
{
    for (Objects *obj : list_)
    {
        Cube *cube = dynamic_cast<Cube *>(obj); // Tentativa de conversão
        if (cube)
        {
            cube->setColors(colorMatrix); // Atualiza se for um Cube
        }
    }
}