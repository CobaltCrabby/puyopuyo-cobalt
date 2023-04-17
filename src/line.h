class Line {
    unsigned int shaderProgram;
    unsigned int VBO, VAO;
    float vertices[6];

    public:
        Line();

        Line (float x1, float y1, float x2, float y2);

        void draw();

        ~Line();
};