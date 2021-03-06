#include "dm.h"

//===============================================================================================================================================
//implementacao de funcoes auxiliares para a biblioteca

//funcao que processa teclas sendo apertadas
void processInput(GLFWwindow* window){
	//se apertar ESC, fecha a janela
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

char* readShader(const char* filename){
	if(filename == NULL){
		return NULL;
	}
	//source code lido do arquivo
	char* source;
	//variavel temporaria usada para ler o arquivo
	char c;
	int size;
	//ponteiro para o arquivo
	std::ifstream inFile(filename, std::ios::ate);

	if(!inFile.is_open()){
		throw std::runtime_error("unable to read shader source code");
	}

	size = inFile.tellg();
	inFile.seekg(0);
	source = (char*) malloc(size*sizeof(char));
	for(int i=0;i<size; i++){
		c = inFile.get();
		source[i] = c;
	}
	source[size-1] = 0;

	return source;
}

void compileShader(unsigned int shader, const char* filename){
	char* shaderSource = readShader(filename);


	glShaderSource(shader, 1, &shaderSource, nullptr);

	free(shaderSource);

	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		throw std::runtime_error(infoLog);
	}
}

//===============================================================================================================================================
//Implementacao de metodos publicos


DisplayManager::DisplayManager(){
	//seta o tamanho da janela como algo impossivel
	dimensions[0]=-1;
	dimensions[1]=-1;

	//inicia glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 0);
}

DisplayManager::~DisplayManager(){
	while(!VAO_array.empty()){
		deregister_VAO(VAO_array[0]);
	}
	glfwTerminate();
}

void DisplayManager::init(int WIDTH, int HEIGHT, const char* title){
	dimensions[0] = WIDTH;
	dimensions[1] = HEIGHT;

	//inicializacao da janela, onde tudo vai ser renderizado
	if(title == nullptr){
		window = glfwCreateWindow(dimensions[0], dimensions[1], "projeto 1", nullptr,nullptr);
	}else{
		window = glfwCreateWindow(dimensions[0], dimensions[1], title, nullptr,nullptr);
	}

	//checagem que a janela foi inicializada
	if(window == nullptr){
		throw std::runtime_error("unable to create glfw window");
	}
	glfwMakeContextCurrent(window);

	//checagem que a biblioteca auxiliar foi carregada corretamente
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		throw std::runtime_error("failed to initialize GLAD");
	}

	//inicializacao da porta de visao
	glViewport(0,0,dimensions[0], dimensions[1]);

	//compilacao de shaders
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertexShader, "shaders/shader.vert");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragmentShader, "shaders/shader.frag");


	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//free the shader memory

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);
	//armazena os valores para normalizar os vertices mais tarde
	int windowSizeLocation = glGetUniformLocation(shaderProgram, "windowSize");
	glUniform2i(windowSizeLocation, dimensions[0], dimensions[1]);

}

void DisplayManager::run(){
	setClearColor(0.0f,0.0f,0.0f);
	int colorLocation = glGetUniformLocation(shaderProgram, "color");
	//glClear(GL_COLOR_BUFFER_BIT);
	while(!glfwWindowShouldClose(window)){
		//processamento de inputs
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT);

		//renderizacao
		for(int i=0;i<VAO_array.size();i++){
			glBindVertexArray(VAO_array[i].VAO_ID);
//			printf("%d:{%f,%f,%f}\n",VAO_array[i].VAO_ID, VAO_array[i].color[0], VAO_array[i].color[1], VAO_array[i].color[2]);
			glUniform3fv(colorLocation, 1, VAO_array[i].color);
			if(VAO_array[i].uses_index){
				glDrawElements(VAO_array[i].drawStyle,VAO_array[i].indexCount,GL_UNSIGNED_INT,0);
			}else{
				glDrawArrays(VAO_array[i].drawStyle, 0, VAO_array[i].indexCount);
			}
		}

		//troca de buffers e busca por callbacks necessarios
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void DisplayManager::register_VAO(VAO_INFO info){
	VAO_array.push_back(info);
}

void DisplayManager::deregister_VAO(VAO_INFO toRemove){
	int i;
	//eh passado o VAO_ID, ao inves do indice a ser desalocado, para que outros objetos so precisem saber seu VAO_ID, nao precisem saber o indice
	//do vector, e possam se des-registrar da fila de desenho
	for(i=0;i<VAO_array.size();i++){
		if(VAO_array[i] == toRemove){
			break;
		}
	}
	if(i == VAO_array.size()){
		return; //ID passado nao foi encontrado, ou nao esta ativo
	}
	VAO_array[i].finish();
	VAO_array.erase(VAO_array.begin()+i);
}

void DisplayManager::update_VAO(VAO_INFO newVAO){
	int i;
	for(i = 0; i < VAO_array.size(); i++){
		if(VAO_array[i] == newVAO){
			//se o ID dos 2 for igual, atualiza os demais valores (cor, contagem de vertices,...)
			VAO_array[i] = newVAO;
			return;
		}
	}
	if(i == VAO_array.size()){
		register_VAO(newVAO);
	}
}

void DisplayManager::registerMouseButtonCallback(void (*mouseFunc)(GLFWwindow*,int,int,int)){
	glfwSetMouseButtonCallback(window,mouseFunc);
}

void DisplayManager::setClearColor(float r, float g, float b){
	glClearColor(r,g,b,1.0f);
}
