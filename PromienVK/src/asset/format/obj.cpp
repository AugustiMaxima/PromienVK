#include <fstream>
#include <sstream>
#include <vector>
#include "../../utils/stdpp.hpp"
#include "obj.hpp"

namespace asset {
	namespace format {
		//float parsing is slow, a prod build model file should store this as binary
		obj::obj(const std::string& fileName) :fileName(fileName) {
			using namespace std;
			ifstream src{ fileName };
			string lineFeed;
			string tag;
			attributes = 0;

			int s = 0;

			while (src) {
				getline(src, lineFeed);
				if (lineFeed[0] == '#') {
					continue;
				}
				istringstream line{ lineFeed };
				line >> tag;

				if (tag[0] == 'v') {
					float buffer;
					for (int i = 0; i < 3; i++) {
						line >> buffer;
						if (tag == "v") {
							vertices.push_back(buffer);
						}
						else if (tag == "vn") {
							normals.push_back(buffer);
						}
						else if (tag == "vt") {
							if(i<2)
								coords.push_back(buffer);
						}
					}
				}
				else if (tag == "f") {
					break;
				}
			} // vertex processing

			istringstream line{ lineFeed };
			line >> tag; //f
			line >> tag; //assembly
			size_t i = 0;
			int t = 0;
			for (; i < tag.length();) {
				if (tag[i] == '/') {
					i++;
					t++;
				}
				else {
					int idx = util::stoi(tag, &i);
					if (t == 1)
						attributes += 2;
					else
						attributes += 3;
				}
			}

			while (src) {
				getline(src, lineFeed);
				if (lineFeed[0] == '#') {
					continue;
				}
				istringstream line{ lineFeed };
				line >> tag;

				if (tag == "f") {
					for (int j = 0; j < 3; j++) {
						line >> tag;
						size_t i = 0;
						int t = 0;
						for (; i < tag.length(); t++) {
							if (tag[i] != '/') {
								int idx = util::stoi(tag, &i);
								float* fs;
								int l = 3;
								switch (t) {
								case 0:
									fs = vertices.data();
									break;
								case 1:
									fs = coords.data();
									l = 2;
									break;
								default:
									fs = normals.data();
									break;
								}
								for (int k = 0; k < l; k++) {
									faces.push_back(fs[(idx - 1) * l + k]);
								}
							}
							i++;
						}
					}
				}
			}
			src.close();
		}

		int obj::getAttributes() {
			return attributes;
		}

		int obj::getVerticeCount() {
			return faces.size() / attributes;
		}

		int obj::getPolygonCount() {
			return faces.size() / attributes / 3;
		}

		void obj::copy(void* vertex, void* indice) {}

		void obj::copy(void* vertex) {
			int copySize = faces.size() * sizeof(float);
			memcpy(vertex, faces.data(), copySize);
		}

		obj::~obj() {

		}
	}
}