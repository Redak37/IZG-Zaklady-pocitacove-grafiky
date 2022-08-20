/*!
 * @file
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <math.h>

#include <student/gpu.h>
#include <student/student_shader.h>
#include <student/uniforms.h>

#ifndef __cplusplus
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(GPUVertexShaderOutput *const      output,
                        GPUVertexShaderInput const *const input,
                        GPU const                         gpu) {
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici
  /// čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních
  /// atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret*
  /// definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce
  /// shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
	Uniforms const uniform = gpu_getUniformsHandle(gpu);

	UniformLocation const viewMatrix = getUniformLocation(gpu, "viewMatrix");
	UniformLocation const projectionMatrix = getUniformLocation(gpu, "projectionMatrix");

	Vec3 const*const pos = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
	Vec3 const*const norm = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);

	Mat4 mat;
	multiply_Mat4_Mat4(&mat, shader_interpretUniformAsMat4(uniform, projectionMatrix), shader_interpretUniformAsMat4(uniform, viewMatrix));

	Vec4 vec;
	copy_Vec3Float_To_Vec4(&vec, pos, 1);

	multiply_Mat4_Vec4(&output->gl_Position, &mat, &vec);

	init_Vec3(vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0), pos->data[0], pos->data[1], pos->data[2]);

	init_Vec3(vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1), norm->data[0], norm->data[1], norm->data[2]);
}



void phong_fragmentShader(GPUFragmentShaderOutput *const      output,
                          GPUFragmentShaderInput const *const input,
                          GPU const                           gpu) {
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici
  /// světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte podle normály povrchu.
  /// V případě, že normála směřuje kolmo vzhůru je difuzní barva čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je difuzní barva čiště zelená.
  /// Difuzní barvu spočtěte lineární interpolací zelené a bíle barvy pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()

	Uniforms const uniform = gpu_getUniformsHandle(gpu);

	Vec3 const*const light = shader_interpretUniformAsVec3(uniform, getUniformLocation(gpu, "lightPosition"));
	Vec3 const*const cam = shader_interpretUniformAsVec3(uniform, getUniformLocation(gpu, "cameraPosition"));

	Vec3 const*const position = fs_interpretInputAttributeAsVec3(gpu, input, 0);
	Vec3 const*const normal = fs_interpretInputAttributeAsVec3(gpu, input, 1);

	Vec3 N;
	normalize_Vec3(&N, normal);

	if (N.data[0] == 0.f && N.data[1] > 0.f && N.data[2] == 0.f)
	{
		output->color.data[0] = 1.f;
		output->color.data[1] = 1.f;
		output->color.data[2] = 1.f;
	}
	else
	{
		Vec3 L;
		L.data[0] = light->data[0] - position->data[0];
		L.data[1] = light->data[1] - position->data[1];
		L.data[2] = light->data[2] - position->data[2];
		normalize_Vec3(&L, &L);

		float dF = L.data[0] * N.data[0] + L.data[1] * N.data[1] + L.data[2] * N.data[2];
		if (dF < 0.f) dF = 0.f;
		else if (dF > 1.f) dF = 1.f;

		if (N.data[1] < 0.f)
		{
			output->color.data[0] = 0.f;
			output->color.data[1] = dF;
			output->color.data[2] = 0.f;
		}
		else
		{
			Vec3 V;
			V.data[0] = position->data[0] - cam->data[0];
			V.data[1] = position->data[1] - cam->data[1];
			V.data[2] = position->data[2] - cam->data[2];
			normalize_Vec3(&V, &V);

			Vec3 R;
			reflect(&R, &V, &N);

			float sF = R.data[0] * L.data[0] + R.data[1] * L.data[1] + R.data[2] * L.data[2];

			if (sF <= 0.f) sF = 0.f;
			else if (sF >= 1.f) sF = 1.f;
			else sF = (float) pow(sF, 40.f);

			output->color.data[0] = max(min((N.data[1] * N.data[1]) * dF + sF, 1.f), 0.f);
			output->color.data[1] = max(min(dF + sF, 1.f), 0.f);
			output->color.data[2] = output->color.data[0];
		}
	}

	output->color.data[3] = 1.f;
}

/// @}
