xof 0303txt 0032
template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}


Mesh {
 102;
 -22.052264;1.000000;5.638068;,
 -19.999998;1.000000;6.000000;,
 -19.999998;1.000000;5.500000;,
 -21.870347;1.000000;5.172144;,
 -23.856574;1.000000;4.596348;,
 -23.530621;1.000000;4.217177;,
 -25.196150;1.000000;3.000000;,
 -24.761187;1.000000;2.753371;,
 -25.908840;1.000000;1.041720;,
 -25.416189;1.000000;0.956307;,
 -25.908840;1.000000;-1.041720;,
 -25.416218;1.000000;-0.956148;,
 -24.761270;1.000000;-2.753228;,
 -25.196150;1.000000;-3.000000;,
 -23.530731;1.000000;-4.217084;,
 -23.856574;1.000000;-4.596348;,
 -21.870541;1.000000;-5.172072;,
 -22.052264;1.000000;-5.638068;,
 -19.999998;1.000000;-5.500000;,
 -19.999998;1.000000;-6.000000;,
 -19.999998;1.000000;-6.000000;,
 -19.999998;0.000000;-6.000000;,
 -22.052264;1.000000;-5.638068;,
 -22.052264;0.000000;-5.638068;,
 -23.856574;1.000000;-4.596348;,
 -23.856574;0.000000;-4.596348;,
 -25.196150;1.000000;-3.000000;,
 -25.196150;0.000000;-3.000000;,
 -25.908840;1.000000;-1.041720;,
 -25.908840;0.000000;-1.041720;,
 -25.908840;1.000000;1.041720;,
 -25.908840;0.000000;1.041720;,
 -25.196150;1.000000;3.000000;,
 -25.196150;0.000000;3.000000;,
 -23.856574;1.000000;4.596348;,
 -23.856574;0.000000;4.596348;,
 -22.052264;1.000000;5.638068;,
 -22.052264;0.000000;5.638068;,
 -19.999998;1.000000;6.000000;,
 -19.999998;0.000000;6.000000;,
 -19.999998;1.000000;6.000000;,
 -19.999998;0.000000;6.000000;,
 -19.999998;1.000000;5.500000;,
 -19.999998;0.000000;5.000000;,
 -19.999998;1.000000;5.500000;,
 -19.999998;0.000000;5.000000;,
 -21.870347;1.000000;5.172144;,
 -20.975426;0.000000;4.903929;,
 -21.913397;0.000000;4.619402;,
 -23.530621;1.000000;4.217177;,
 -22.777840;0.000000;4.157352;,
 -23.535523;0.000000;3.535541;,
 -24.761187;1.000000;2.753371;,
 -24.157341;0.000000;2.777857;,
 -24.619392;0.000000;1.913421;,
 -25.416189;1.000000;0.956307;,
 -24.903923;0.000000;0.975454;,
 -24.999997;0.000000;0.000000;,
 -25.416218;1.000000;-0.956148;,
 -24.903923;0.000000;-0.975454;,
 -24.761270;1.000000;-2.753228;,
 -24.619392;0.000000;-1.913421;,
 -24.157341;0.000000;-2.777857;,
 -23.530731;1.000000;-4.217084;,
 -23.535523;0.000000;-3.535540;,
 -22.777840;0.000000;-4.157352;,
 -21.870541;1.000000;-5.172072;,
 -21.913397;0.000000;-4.619402;,
 -20.975426;0.000000;-4.903929;,
 -19.999998;1.000000;-5.500000;,
 -19.999998;0.000000;-5.000000;,
 -19.999998;0.000000;5.000000;,
 -19.999998;0.000000;6.000000;,
 -22.052264;0.000000;5.638068;,
 -20.975426;0.000000;4.903929;,
 -21.913397;0.000000;4.619402;,
 -23.856574;0.000000;4.596348;,
 -22.777840;0.000000;4.157352;,
 -23.535523;0.000000;3.535541;,
 -25.196150;0.000000;3.000000;,
 -24.157341;0.000000;2.777857;,
 -24.619392;0.000000;1.913421;,
 -25.908840;0.000000;1.041720;,
 -24.903923;0.000000;0.975454;,
 -24.999997;0.000000;0.000000;,
 -24.903923;0.000000;-0.975454;,
 -25.908840;0.000000;-1.041720;,
 -24.619392;0.000000;-1.913421;,
 -24.157341;0.000000;-2.777857;,
 -25.196150;0.000000;-3.000000;,
 -23.535523;0.000000;-3.535540;,
 -22.777840;0.000000;-4.157352;,
 -23.856574;0.000000;-4.596348;,
 -21.913397;0.000000;-4.619402;,
 -20.975426;0.000000;-4.903929;,
 -22.052264;0.000000;-5.638068;,
 -19.999998;0.000000;-5.000000;,
 -19.999998;0.000000;-6.000000;,
 -19.999998;1.000000;-5.500000;,
 -19.999998;0.000000;-5.000000;,
 -19.999998;1.000000;-6.000000;,
 -19.999998;0.000000;-6.000000;;
 90;
 3;0,1,2;,
 3;0,2,3;,
 3;4,0,3;,
 3;4,3,5;,
 3;6,4,5;,
 3;6,5,7;,
 3;8,6,7;,
 3;8,7,9;,
 3;10,8,9;,
 3;10,9,11;,
 3;10,11,12;,
 3;13,10,12;,
 3;13,12,14;,
 3;15,13,14;,
 3;15,14,16;,
 3;17,15,16;,
 3;18,17,16;,
 3;18,19,17;,
 3;20,21,22;,
 3;21,23,22;,
 3;22,23,24;,
 3;23,25,24;,
 3;24,25,26;,
 3;25,27,26;,
 3;26,27,28;,
 3;27,29,28;,
 3;28,29,30;,
 3;29,31,30;,
 3;30,31,32;,
 3;31,33,32;,
 3;32,33,34;,
 3;33,35,34;,
 3;34,35,36;,
 3;35,37,36;,
 3;36,37,38;,
 3;37,39,38;,
 3;40,41,42;,
 3;41,43,42;,
 3;44,45,46;,
 3;45,47,46;,
 3;47,48,46;,
 3;46,48,49;,
 3;48,50,49;,
 3;50,51,49;,
 3;49,51,52;,
 3;51,53,52;,
 3;53,54,52;,
 3;52,54,55;,
 3;54,56,55;,
 3;56,57,55;,
 3;55,57,58;,
 3;57,59,58;,
 3;58,59,60;,
 3;59,61,60;,
 3;61,62,60;,
 3;60,62,63;,
 3;62,64,63;,
 3;64,65,63;,
 3;63,65,66;,
 3;65,67,66;,
 3;67,68,66;,
 3;68,69,66;,
 3;68,70,69;,
 3;71,72,73;,
 3;74,71,73;,
 3;75,74,73;,
 3;75,73,76;,
 3;77,75,76;,
 3;78,77,76;,
 3;78,76,79;,
 3;80,78,79;,
 3;81,80,79;,
 3;81,79,82;,
 3;83,81,82;,
 3;84,83,82;,
 3;85,84,82;,
 3;85,82,86;,
 3;87,85,86;,
 3;88,87,86;,
 3;88,86,89;,
 3;90,88,89;,
 3;91,90,89;,
 3;91,89,92;,
 3;93,91,92;,
 3;94,93,92;,
 3;94,92,95;,
 3;96,94,95;,
 3;96,95,97;,
 3;98,99,100;,
 3;99,101,100;;

 MeshNormals {
  102;
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;-1.000000;,
  -0.342020;0.000000;-0.939693;,
  -0.342020;0.000000;-0.939693;,
  -0.642788;0.000000;-0.766044;,
  -0.642788;0.000000;-0.766044;,
  -0.866025;0.000000;-0.500000;,
  -0.866025;0.000000;-0.500000;,
  -0.984808;0.000000;-0.173648;,
  -0.984808;0.000000;-0.173648;,
  -0.984808;0.000000;0.173648;,
  -0.984808;0.000000;0.173648;,
  -0.866025;0.000000;0.500000;,
  -0.866025;0.000000;0.500000;,
  -0.642788;0.000000;0.766044;,
  -0.642788;0.000000;0.766044;,
  -0.342020;0.000000;0.939693;,
  -0.342020;0.000000;0.939693;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  0.000000;0.447213;-0.894427;,
  0.000000;0.447213;-0.894427;,
  0.304141;0.447213;-0.841129;,
  0.174490;0.447213;-0.877242;,
  0.342279;0.447213;-0.826345;,
  0.574181;0.447213;-0.685796;,
  0.496915;0.447213;-0.743690;,
  0.632454;0.447213;-0.632457;,
  0.774280;0.447213;-0.447762;,
  0.743688;0.447213;-0.496918;,
  0.826343;0.447213;-0.342283;,
  0.880799;0.447213;-0.155543;,
  0.877241;0.447213;-0.174495;,
  0.894427;0.447213;0.000000;,
  0.880804;0.447213;0.155517;,
  0.877241;0.447213;0.174495;,
  0.774293;0.447213;0.447739;,
  0.826343;0.447213;0.342283;,
  0.743688;0.447213;0.496918;,
  0.574199;0.447213;0.685781;,
  0.632454;0.447213;0.632457;,
  0.496915;0.447213;0.743690;,
  0.304173;0.447213;0.841118;,
  0.342279;0.447213;0.826345;,
  0.174490;0.447213;0.877242;,
  0.000000;0.447213;0.894427;,
  0.000000;0.447213;0.894427;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;;
  90;
  3;0,1,2;,
  3;0,2,3;,
  3;4,0,3;,
  3;4,3,5;,
  3;6,4,5;,
  3;6,5,7;,
  3;8,6,7;,
  3;8,7,9;,
  3;10,8,9;,
  3;10,9,11;,
  3;10,11,12;,
  3;13,10,12;,
  3;13,12,14;,
  3;15,13,14;,
  3;15,14,16;,
  3;17,15,16;,
  3;18,17,16;,
  3;18,19,17;,
  3;20,21,22;,
  3;21,23,22;,
  3;22,23,24;,
  3;23,25,24;,
  3;24,25,26;,
  3;25,27,26;,
  3;26,27,28;,
  3;27,29,28;,
  3;28,29,30;,
  3;29,31,30;,
  3;30,31,32;,
  3;31,33,32;,
  3;32,33,34;,
  3;33,35,34;,
  3;34,35,36;,
  3;35,37,36;,
  3;36,37,38;,
  3;37,39,38;,
  3;40,41,42;,
  3;41,43,42;,
  3;44,45,46;,
  3;45,47,46;,
  3;47,48,46;,
  3;46,48,49;,
  3;48,50,49;,
  3;50,51,49;,
  3;49,51,52;,
  3;51,53,52;,
  3;53,54,52;,
  3;52,54,55;,
  3;54,56,55;,
  3;56,57,55;,
  3;55,57,58;,
  3;57,59,58;,
  3;58,59,60;,
  3;59,61,60;,
  3;61,62,60;,
  3;60,62,63;,
  3;62,64,63;,
  3;64,65,63;,
  3;63,65,66;,
  3;65,67,66;,
  3;67,68,66;,
  3;68,69,66;,
  3;68,70,69;,
  3;71,72,73;,
  3;74,71,73;,
  3;75,74,73;,
  3;75,73,76;,
  3;77,75,76;,
  3;78,77,76;,
  3;78,76,79;,
  3;80,78,79;,
  3;81,80,79;,
  3;81,79,82;,
  3;83,81,82;,
  3;84,83,82;,
  3;85,84,82;,
  3;85,82,86;,
  3;87,85,86;,
  3;88,87,86;,
  3;88,86,89;,
  3;90,88,89;,
  3;91,90,89;,
  3;91,89,92;,
  3;93,91,92;,
  3;94,93,92;,
  3;94,92,95;,
  3;96,94,95;,
  3;96,95,97;,
  3;98,99,100;,
  3;99,101,100;;
 }

 MeshTextureCoords {
  102;
  0.992788;0.041397;,
  0.992049;0.000343;,
  0.992049;0.023410;,
  0.992728;0.059466;,
  0.993333;0.101286;,
  0.993240;0.114275;,
  0.993687;0.173041;,
  0.993576;0.180804;,
  0.993860;0.251301;,
  0.993741;0.253878;,
  0.993860;0.331612;,
  0.993741;0.329030;,
  0.993576;0.402103;,
  0.993687;0.409872;,
  0.993240;0.468634;,
  0.993333;0.481627;,
  0.992728;0.523443;,
  0.992788;0.541517;,
  0.992049;0.559504;,
  0.992049;0.582570;,
  0.992049;0.582570;,
  1.000000;0.582914;,
  0.992788;0.541517;,
  1.000000;0.541763;,
  0.993333;0.481627;,
  1.000000;0.481790;,
  0.993687;0.409872;,
  1.000000;0.409965;,
  0.993860;0.331612;,
  1.000000;0.331642;,
  0.993860;0.251301;,
  1.000000;0.251271;,
  0.993687;0.173041;,
  1.000000;0.172949;,
  0.993333;0.101286;,
  1.000000;0.101123;,
  0.992788;0.041397;,
  1.000000;0.041150;,
  0.992049;0.000343;,
  1.000000;0.000000;,
  0.992049;0.000343;,
  1.000000;0.000000;,
  0.992049;0.023410;,
  1.000000;0.046478;,
  0.992049;0.023410;,
  1.000000;0.046478;,
  0.992728;0.059466;,
  0.000000;0.061788;,
  1.000000;0.083696;,
  0.993240;0.114275;,
  1.000000;0.110927;,
  1.000000;0.142350;,
  0.993576;0.180804;,
  1.000000;0.176969;,
  1.000000;0.213893;,
  0.993741;0.253878;,
  1.000000;0.252308;,
  1.000000;0.291457;,
  0.993741;0.329030;,
  1.000000;0.330605;,
  0.993576;0.402103;,
  1.000000;0.369021;,
  1.000000;0.405944;,
  0.993240;0.468634;,
  1.000000;0.440563;,
  1.000000;0.471987;,
  0.992728;0.523443;,
  1.000000;0.499218;,
  0.000000;0.521126;,
  0.992049;0.559504;,
  1.000000;0.536435;,
  1.000000;0.046478;,
  1.000000;0.000000;,
  1.000000;0.041150;,
  0.000000;0.061788;,
  1.000000;0.083696;,
  1.000000;0.101123;,
  1.000000;0.110927;,
  1.000000;0.142350;,
  1.000000;0.172949;,
  1.000000;0.176969;,
  1.000000;0.213893;,
  1.000000;0.251271;,
  1.000000;0.252308;,
  1.000000;0.291457;,
  1.000000;0.330605;,
  1.000000;0.331642;,
  1.000000;0.369021;,
  1.000000;0.405944;,
  1.000000;0.409965;,
  1.000000;0.440563;,
  1.000000;0.471987;,
  1.000000;0.481790;,
  1.000000;0.499218;,
  0.000000;0.521126;,
  1.000000;0.541763;,
  1.000000;0.536435;,
  1.000000;0.582914;,
  0.992049;0.559504;,
  1.000000;0.536435;,
  0.992049;0.582570;,
  1.000000;0.582914;;
 }

 MeshMaterialList {
  1;
  90;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;

  Material {
   0.878000;0.949000;1.000000;1.000000;;
   10.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "";
   }
  }
 }
}