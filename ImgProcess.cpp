#include "ImgProcess.h"
#define PROFILE_NAME    "/home/cj/workspace/avmsetting.xml"
#include "./sxmlc/sxmlc.h"
#include "./sxmlc/sxmlsearch.h"

AreaSettings    m_as[MAX_CAMERAS] = {

};
ImgProcess::ImgProcess(int id): m_nId(id)
{

    LoadSettings(PROFILE_NAME);
}
ImgProcess::~ImgProcess()
{
    SaveSettings(PROFILE_NAME);
}
bool ImgProcess::LoadSettings(const char* path)
{
    XMLNode *node, *node1, *node2, *node3;
    XMLDoc doc;
    XMLSearch search;
    XMLDoc_init(&doc);
    if (!XMLDoc_parse_file_DOM(C2SX(path), &doc)) {
            printf("Error while loading\n");
            return false;
    }
    XMLSearch_init(&search);
    XMLSearch_search_set_tag(&search, C2SX("area"));
    XMLSearch_search_add_attribute(&search, C2SX("ID"), C2SX("*"), true);



    node = XMLDoc_root(&doc);
    while ((node = XMLSearch_next(node, &search)) != NULL) {
            printf("Found match: ");
            XMLNode_print(node, stdout, NULL, NULL, false, 0, 0);
            printf("\n");
            XMLSearch search1, search2;
            XMLSearch_init(&search1);
            XMLSearch_init(&search2);

            XMLSearch_search_set_tag(&search1, C2SX("range"));
            node2 = XMLSearch_next(node, &search1);
            if (node2){
                printf("range = %s \n", node2->text);
            }
            XMLSearch_search_set_tag(&search1, C2SX("fec"));
            node2 = XMLSearch_next(node1, &search1);
            if (node2) {
                XMLSearch_search_set_tag(&search2, C2SX("input"));
                node3 = XMLSearch_next(node2, &search2);
                if (node3)
                    printf("input = %s \n", node3->text);

            }
            XMLSearch_search_set_tag(&search1, C2SX("homo"));
            XMLSearch_search_add_attribute(&search1, C2SX("ID"), C2SX("*"), true);
            while((node2 = XMLSearch_next(node1, &search1)) != NULL) {

                printf("homo ID = %s \n", node2->text);
            }

    }
    XMLSearch_free(&search, false);
    //
    XMLDoc_free(&doc);
}

bool ImgProcess::SaveSettings(const char* path)
{
    XMLNode *node, *node1, *node2;
    XMLDoc doc;
    FILE* f;

    XMLDoc_init(&doc);

    node = XMLNode_alloc();
    XMLNode_set_tag(node, C2SX("xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\""));
    XMLNode_set_type(node, TAG_INSTR);
    XMLDoc_add_node(&doc, node);

    node = XMLNode_alloc();
    XMLNode_set_tag(node, C2SX("AVM Settings File"));
    XMLNode_set_type(node, TAG_COMMENT);
    XMLDoc_add_node(&doc, node);

    node = XMLNode_alloc();
    XMLNode_set_tag(node, C2SX("max_camera"));
    XMLNode_set_text(node, C2SX("4"));
    XMLDoc_add_node(&doc, node);

    node = XMLNode_alloc();
    XMLNode_set_tag(node, C2SX("area"));
    XMLNode_set_attribute(node, C2SX("ID"), C2SX("0"));
    XMLDoc_add_node(&doc, node);

    node1 = XMLNode_alloc();
    XMLNode_set_tag(node1, C2SX("range"));
    XMLNode_set_attribute(node1, C2SX("type"), C2SX("RECT"));
    XMLNode_set_text(node1, C2SX("0,0,1, 0.45"));
    XMLNode_add_child(node, node1);

    node1 = XMLNode_alloc();
    XMLNode_set_tag(node1, C2SX("fec"));
    XMLNode_add_child(node, node1);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("input"));
    XMLNode_set_attribute(node2, C2SX("type"), C2SX("RECT"));
    XMLNode_set_text(node2, C2SX("0,0,1279, 1023"));
    XMLNode_add_child(node1, node2);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("center"));
    XMLNode_set_attribute(node2, C2SX("type"), C2SX("POINT"));
    XMLNode_set_text(node2, C2SX("640, 512"));
    XMLNode_add_child(node1, node2);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("radius"));
    XMLNode_set_attribute(node2, C2SX("type"), C2SX("SIZE"));
    XMLNode_set_text(node2, C2SX("640, 512"));
    XMLNode_add_child(node1, node2);
    /*.... fec ....*/

    node1 = XMLNode_alloc();
    XMLNode_set_tag(node1, C2SX("homo"));
    XMLNode_set_attribute(node1, C2SX("ID"), C2SX("0"));
    XMLNode_add_child(node, node1);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("fpt"));
    XMLNode_set_attribute(node2, C2SX("ID"), C2SX("0"));
    XMLNode_set_text(node2, C2SX("0,0,0,0"));
    XMLNode_add_child(node1, node2);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("fpt"));
    XMLNode_set_attribute(node2, C2SX("ID"), C2SX("1"));
    XMLNode_set_text(node2, C2SX("0,0,0.5,0.5"));
    XMLNode_add_child(node1, node2);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("fps"));
    XMLNode_set_attribute(node2, C2SX("ID"), C2SX("0"));
    XMLNode_set_text(node2, C2SX("0,0,0.5,0.5"));
    XMLNode_add_child(node1, node2);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("fps"));
    XMLNode_set_attribute(node2, C2SX("ID"), C2SX("1"));
    XMLNode_set_text(node2, C2SX("0,0,0.5,0.5"));
    XMLNode_add_child(node1, node2);

    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("h"));
    XMLNode_set_attribute(node2, C2SX("type"), C2SX("double array"));
    XMLNode_set_text(node2, C2SX("1,0,0, 0,1,0, 0,0,1"));
    XMLNode_add_child(node1, node2);


    node2 = XMLNode_alloc();
    XMLNode_set_tag(node2, C2SX("range"));
    XMLNode_set_attribute(node2, C2SX("type"), C2SX("RECT"));
    XMLNode_set_text(node2, C2SX("1,0,0, 0,1,0, 0,0,1"));
    XMLNode_add_child(node1, node2);

    node1 = XMLNode_alloc();
    XMLNode_set_tag(node1, C2SX("homo"));
    XMLNode_set_attribute(node1, C2SX("ID"), C2SX("1"));
    XMLNode_add_child(node, node1);

    node1 = XMLNode_alloc();
    XMLNode_set_tag(node1, C2SX("homo"));
    XMLNode_set_attribute(node1, C2SX("ID"), C2SX("2"));
    XMLNode_add_child(node, node1);

    node1 = XMLNode_alloc();
    XMLNode_set_tag(node1, C2SX("homo"));
    XMLNode_set_attribute(node1, C2SX("ID"), C2SX("3"));
    XMLNode_add_child(node, node1);

    f = fopen(path, "w+t");
    if (f == NULL) f = stdout;
    XMLDoc_print(&doc, f, C2SX("\n"), C2SX("    "), false, 0, 4);
    if (f != stdout) fclose(f);

    XMLDoc_free(&doc);
    return true;
}

int ImgProcess::updateUv(vector <QVector2D> &uv)
{
    return 0;
}
