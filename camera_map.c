// Copyleft 2013 Wenda_Lu @ NJCLW
// You can freely use, modify and distribute this program
// Please keep this declaration

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "camera_map.h"


// 从文件读取映射表。此文件由相机校正模块生成
void table_read (mapTable **p_mt)
{
	mapTable *p = malloc (WIDTH_OUT * HEIGHT_OUT * sizeof(mapTable));
	if (p == NULL)
		printf ("ERROR: memory allocation of mapping table failure\n");
	
	*p_mt = p;
	
	// 打开并读取映射表
	int fp = open ("480.dat", O_RDONLY);
	
	if (fp <= 0)
		printf ("ERROR: opening mapping table file failure\n");
	
	read (fp, (void *)*p_mt, WIDTH_OUT * HEIGHT_OUT * sizeof(mapTable));
	close (fp);
}


// 生成模拟映射表, 供调试与性能测试使用
// 上下左右区域直接搬移输入图像, 四个对角交叉区域采用随机化偏移量
int table_gen (mapTable **p_mt)
{
	mapTable *p = malloc (WIDTH_OUT * HEIGHT_OUT * sizeof(mapTable));
	*p_mt = p;
	mapTable t;
	
	int top_left = 0,
	top = 0,
	top_right = 0,
	mid_left = WIDTH_IN * (HEIGHT_IN - 100 - (HEIGHT_OUT - 2 * DIV)) / 2,
	mid = 0,
	mid_right = WIDTH_IN * (HEIGHT_IN - 100 - (HEIGHT_OUT - 2 * DIV)) / 2 -DIV,
	bottom_left = 0,
	bottom = WIDTH_IN * (HEIGHT_IN - DIV - 100),
	bottom_right = 0;
	
	int t_l = 0,
	tp = 0,
	t_r = 0,
	m_l = (HEIGHT_IN - 100 - (HEIGHT_OUT - 2 * DIV)) / 2,
	md = 0,
	m_r = (HEIGHT_IN - 100 - (HEIGHT_OUT - 2 * DIV)) / 2,
	b_l = 0,
	bm = HEIGHT_IN - DIV - 100,
	b_r = 0;
	
	int u, v;
	// 映射表的维度等于输出图像的维度, 输出图像中的每一个像素对应映射表中的一个单元
	for (u = 0; u < HEIGHT_OUT; u++)
	{
		for (v = 0; v < WIDTH_OUT; v++)
		{
			if (u < DIV)
			{
				// 左上区域
				if (v < DIV)
				{
					t.cam_id_1 = 4;		// 第一个摄像头的ID
					t.cam_id_2 = 1;		// 第二个摄像头的ID. 如果输出来自两个摄像头, 按顺时针顺序排列. 如果只来自一个摄像头, 此字段值为255
					t.ratio = 127;		// 第一个摄像头图像像素值所占的比例
					t.offset_1 = rand() % (WIDTH_IN*HEIGHT_IN);		// 随机生成针对第一个摄像头图像缓冲区的偏移量
					t.offset_2 = rand() % (WIDTH_IN*HEIGHT_IN);		// 随机生成针对第二个摄像头图像缓冲区的偏移量
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
				// 上方区域
				else if (v < WIDTH_OUT - DIV)
				{
					t.cam_id_1 = 1;
					t.cam_id_2 = 0;
					t.ratio = 255;
					
					if ((top - tp * WIDTH_IN) / (WIDTH_OUT - DIV) == 1)
						top = (++tp) * WIDTH_IN + DIV;
					
					t.offset_1 = top++;		
					t.offset_2 = 0;
					
					memcpy (p, &t, sizeof(mapTable));
					p ++;	
				}
				// 右上区域
				else
				{
					t.cam_id_1 = 1;
					t.cam_id_2 = 2;
					t.ratio = 127;
					t.offset_1 = rand() % (WIDTH_IN*HEIGHT_IN);
					t.offset_2 = rand() % (WIDTH_IN*HEIGHT_IN);
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
			}
			else if (u < HEIGHT_OUT - DIV)
			{
				// 左中区域
				if (v < DIV)
				{
					t.cam_id_1 = 4;
					t.cam_id_2 = 0;
					t.ratio = 255;
					
					if ((mid_left - m_l * WIDTH_IN) / DIV == 1)
						mid_left = (++m_l) * WIDTH_IN;
					t.offset_1 = mid_left++;
					
					t.offset_2 = 0;
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
				// 正中区域
				else if (v < WIDTH_OUT - DIV)
				{
					t.cam_id_1 = 0;
					t.cam_id_2 = 0;
					t.ratio = 0;
					t.offset_1 = 0;
					t.offset_2 = 0;
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
				// 右中区域
				else
				{
					t.cam_id_1 = 2;
					t.cam_id_2 = 0;
					t.ratio = 255;
					
					if (mid_right - m_r * WIDTH_IN == 0)
						mid_right = (++ m_r) * WIDTH_IN - DIV;
					t.offset_1 = mid_right ++;
					
					t.offset_2 = 0;
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
			}
			else
			{
				// 左下区域
				if (v < DIV)
				{
					t.cam_id_1 = 3;
					t.cam_id_2 = 4;
					t.ratio = 127;
					t.offset_1 = rand() % (WIDTH_IN*HEIGHT_IN);
					t.offset_2 = rand() % (WIDTH_IN*HEIGHT_IN);
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
				// 下方区域
				else if (v < WIDTH_OUT - DIV)
				{
					t.cam_id_1 = 3;
					t.cam_id_2 = 0;
					t.ratio = 255;
					
					if ((bottom - bm * WIDTH_IN) / (WIDTH_OUT - DIV) == 1)
						bottom = (++bm) * WIDTH_IN + DIV;
					t.offset_1 = bottom++;				
					
					t.offset_2 = 0;
					memcpy (p, &t, sizeof(mapTable));
					p ++;
				}
				// 右下区域
				else
				{
					t.cam_id_1 = 2;
					t.cam_id_2 = 3;
					t.ratio = 127;
					t.offset_1 = rand() % (WIDTH_IN*HEIGHT_IN);
					t.offset_2 = rand() % (WIDTH_IN*HEIGHT_IN);
					memcpy (p, &t, sizeof(mapTable));
					p ++;	
				}
			}
		}
	}
	return 0;
}


// 预处理映射表，提高实时性能。只需提前处理一次
int table_preproc ( BYTE *top_buff,					// 上方摄像头输入图像缓冲
				    BYTE *right_buff,				// 右方摄像头输入图像缓冲
					BYTE *bottom_buff,				// 下方摄像头输入图像缓冲
					BYTE *left_buff,				// 左方摄像头输入图像缓冲
					BYTE *yuv_out,					// 输出图像缓冲
					mapTable *p_map_table,			// 原始映射表
					PBYTE **type_1_addr_in,			// 输入像素地址类型1, 表示源自单独像素
					PBYTE **type_2_addr_in,			// 输入像素地址类型2, 表示源自两个像素, 两像素相邻存储
					PBYTE **type_1_addr_out,		// 输出像素地址类型1, 表示源自单独像素
					PBYTE **type_2_addr_out,		// 输出像素地址类型2, 表示源自两个像素, 读取相邻两地址
					UINT *type_1_counter,			// 类型1像素计数器
					UINT *type_2_counter,			// 类型2像素计数器
					BYTE **ratio,					// 混合比例, 与类型2配合使用
					UINT size_out )					// 输出尺寸, 暂时没什么用
{
	mapTable *p_table = p_map_table;	// 取得原始映射表指针
	
	// 根据交叉区域所占比例调整内存分配大小, 测试时应预留足够的空间
	// 后期可预先循环一次得到类型1与类型2像素点的数目精确分配内存空间
	// 这里暂时选用固定大小
	PBYTE *i_addr_1 = malloc (size_out * 4);
	PBYTE *i_addr_2 = malloc (size_out * 6);
	PBYTE *o_addr_1 = malloc (size_out * 4);
	PBYTE *o_addr_2 = malloc (size_out * 4);
	if (i_addr_1 == NULL || i_addr_2 == NULL || o_addr_1 == NULL || o_addr_2 == NULL)
	{
		printf ("ERROR: memory allocation of input or output addresses failure\n");
		return -1;
	}
	
	BYTE *rt = malloc (size_out);
	if (rt == NULL)
	{
		printf ("ERROR: memory allocation of intersection ratio failure\n");
		return -1;
	}
	
	// 为实参赋值
	*type_1_addr_in = i_addr_1;
	*type_2_addr_in = i_addr_2;
	*type_1_addr_out = o_addr_1;
	*type_2_addr_out = o_addr_2;
	*ratio = rt;
	
	BYTE cid_1, cid_2;		// 摄像头ID
	BYTE *p_m1, *p_m2;		// 临时指针
	UINT ofs_1, ofs_2;		// 输入偏移
	
	// 计数器清零
	*type_1_counter = 0;
	*type_2_counter = 0;
	
	// 取得4幅输入图像的Y, C分量首地址
	BYTE *cam_Y[4] = {top_buff, right_buff, bottom_buff, left_buff};
	BYTE *cam_C[4] = {top_buff + WIDTH_IN * HEIGHT_IN, right_buff + WIDTH_IN * HEIGHT_IN, bottom_buff + WIDTH_IN * HEIGHT_IN, left_buff + WIDTH_IN * HEIGHT_IN};
	
	// 取得输出图像的Y, C分量首地址
	BYTE *Y_out = yuv_out;
	BYTE *C_out = yuv_out + size_out;
	
	UINT i;
	// 预处理Y通道
	for (i = 0; i < size_out; ++ i)
	{
		// 获取摄像头ID
		cid_1 = p_table -> cam_id_1;
		cid_2 = p_table -> cam_id_2;
		
		// 检查偏移量不大于图像大小
		if ((p_table -> offset_1) >= WIDTH_IN * HEIGHT_IN || (p_table -> offset_2) >= WIDTH_IN * HEIGHT_IN)
		{
			printf ("ERROR: offset oversized\n");
			return -1;
		}
		
		// 类型1, 摄像头1 ID有效并且摄像头2 ID无效, 表示源自单独像素
		if (cid_2 == 0 && cid_1 != 0)
		{
			*(i_addr_1 ++) = cam_Y[cid_1-1] + (p_table -> offset_1);	// Y分量输入地址
			*(o_addr_1 ++) = Y_out + i;									// Y分量输出地址 (映射表与输出图像成对应关系)
			
			++ *type_1_counter;											// 类型1计数器递增
		}
		
		// 类型2, 摄像头1 ID与摄像头2 ID全部有效, 表示源自两个像素
		else if (cid_1 != 0 && cid_2 != 0)
		{
			*(i_addr_2 ++) = cam_Y[cid_1-1] + (p_table -> offset_1);	// Y1分量输入地址1, 对应摄像头1
			*(i_addr_2 ++) = cam_Y[cid_2-1] + (p_table -> offset_2);	// Y2分量输入地址2, 对应摄像头2
			*(o_addr_2 ++) = Y_out + i;									// 混合Y分量输出地址
			
			*(rt ++) = p_table -> ratio;								// 混合比例
			
			++ *type_2_counter;											// 类型2计数器递增
		}
		// 条件分支结束
		++ p_table;
		
		// 类型0, 摄像头1 ID与摄像头2 ID均无效, 暂不进行处理
		// else
		// {
		//		;
		// }
	}
	// 循环结束
	
	p_table = p_map_table;		// 重置原始映射表指针
	
	// 预处理C分量
	for (i = 0; i < size_out; ++ i)
	{
		// 隔行隔列采样
		if (i % 2 == 0 && (i / WIDTH_OUT) % 2 == 0)
		{	
			// 获取摄像头ID
			cid_1 = p_table -> cam_id_1;
			cid_2 = p_table -> cam_id_2;
		
			// 类型1, 摄像头1 ID有效并且摄像头2 ID无效, 表示源自单独像素
			if (cid_2 == 0 && cid_1 != 0)
			{
				ofs_1 = p_table -> offset_1;	// 获取Y输入分量偏移
				
				// 通过Y输入分量偏移变换C输入分量偏移
				if ( ofs_1 % 2 == 0 && (ofs_1 / WIDTH_IN) % 2 == 0 )				// 偶数列, 偶数行
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN;	
				
				else if ( ofs_1 % 2 != 0 && (ofs_1 / WIDTH_IN) % 2 == 0 )			// 奇数列, 偶数行
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN - 1;
				
				else if ( ofs_1 % 2 != 0 && (ofs_1 / WIDTH_IN) % 2 != 0 )			// 奇数列, 奇数行
				{
					ofs_1 -= WIDTH_IN;
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN - 1;
				}
				
				else																// 偶数列, 奇数行
				{
					ofs_1 -= WIDTH_IN;
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN;
				}
				
				*(i_addr_1 ++) = p_m1;												// U分量输入地址
				*(i_addr_1 ++) = ++ p_m1;											// V分量输入地址

				*(o_addr_1 ++) = C_out + i - i / WIDTH_OUT / 2 * WIDTH_OUT;			// U分量输出地址
				*(o_addr_1 ++) = C_out + i - i / WIDTH_OUT / 2 * WIDTH_OUT + 1;		// V分量输出地址
				
				(*type_1_counter) += 2;												// 类型1计数器递增
			}
		
			else if (cid_1 != 0 && cid_2 != 0)
			{
				ofs_1 = p_table -> offset_1;		// 获取Y1输入分量偏移
				ofs_2 = p_table -> offset_2;		// 获取Y2输入分量偏移
				
				// 通过Y1输入分量偏移变换C1输入分量偏移
				if ( ofs_1 % 2 == 0 && (ofs_1 / WIDTH_IN) % 2 == 0 )				// 偶数列, 偶数行
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN;	
				
				else if ( ofs_1 % 2 != 0 && (ofs_1 / WIDTH_IN) % 2 == 0 )			// 奇数列, 偶数行
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN - 1;
				
				else if ( ofs_1 % 2 != 0 && (ofs_1 / WIDTH_IN) % 2 != 0 )			// 奇数列, 奇数行
				{
					ofs_1 -= WIDTH_IN;
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN - 1;
				}		
				else																// 偶数列, 奇数行
				{
					ofs_1 -= WIDTH_IN;
					p_m1 = cam_C[cid_1-1] + ofs_1 - ofs_1 / WIDTH_IN / 2 * WIDTH_IN;
				}
				
				// 通过Y2输入分量偏移变换C2输入分量偏移
				if ( ofs_2 % 2 == 0 && (ofs_2 / WIDTH_IN) % 2 == 0 )				// 偶数列, 偶数行
					p_m2 = cam_C[cid_2-1] + ofs_2 - ofs_2 / WIDTH_IN / 2 * WIDTH_IN;	
				
				else if ( ofs_2 % 2 != 0 && (ofs_2 / WIDTH_IN) % 2 == 0 )			// 奇数列, 偶数行
					p_m2 = cam_C[cid_2-1] + ofs_2 - ofs_2 / WIDTH_IN / 2 * WIDTH_IN - 1;
				
				else if ( ofs_2 % 2 != 0 && (ofs_2 / WIDTH_IN) % 2 != 0 )			// 奇数列, 奇数行
				{
					ofs_2 -= WIDTH_IN;
					p_m2 = cam_C[cid_2-1] + ofs_2 - ofs_2 / WIDTH_IN / 2 * WIDTH_IN - 1;
				}	
				else																// 偶数列, 奇数行
				{
					ofs_2 -= WIDTH_IN;
					p_m2 = cam_C[cid_2-1] + ofs_2 - ofs_2 / WIDTH_IN / 2 * WIDTH_IN;
				}
			
				*(i_addr_2 ++) = p_m1;					// U1分量输入地址
				*(i_addr_2 ++) = p_m2;					// U2分量输入地址

				*(i_addr_2 ++) = ++ p_m1;				// V1分量输入地址
				*(i_addr_2 ++) = ++ p_m2;				// V2分量输入地址
			
				*(o_addr_2 ++) = C_out + i - i / WIDTH_OUT / 2 * WIDTH_OUT;			// U分量输出地址
				*(o_addr_2 ++) = C_out + i - i / WIDTH_OUT / 2 * WIDTH_OUT + 1;		// V分量输出地址
			
				*(rt ++) = p_table -> ratio;			// U分量混合比例
				*(rt ++) = p_table -> ratio;			// V分量混合比例
				
				(*type_2_counter) += 2;					// 类型2计数器递增
			
			}
			// 条件分支结束
		
			// 类型0, 摄像头ID_1与摄像头ID_2均无效, 暂不进行处理
			// else
			// {
			//		;
			// }
		}
		// 条件分支结束
		
		p_table ++;
	}
	// 循环结束
	return 0;
}


// 写入输出缓冲（请把此函数放在主循环里）
void write_yuv_output ( PBYTE *type_1_addr_in,		// 输入像素地址类型1, 表示源自单独像素
						PBYTE *type_2_addr_in,		// 输入像素地址类型2, 表示源自两个像素, 两像素相邻存储
						PBYTE *type_1_addr_out,		// 输出像素地址类型1, 表示源自单独像素
						PBYTE *type_2_addr_out,		// 输出像素地址类型2, 表示源自两个像素, 读取相邻两地址
						UINT type_1_counter,		// 类型1像素计数器
						UINT type_2_counter,		// 类型2像素计数器
						BYTE *ratio )				// 混合比例, 与类型2配合使用
						
{
	register BYTE r;		// 临时存储混合比例

	// 写入像素类型1 (源自单独像素)
	for (;type_1_counter; -- type_1_counter)
		**type_1_addr_out ++ = **type_1_addr_in ++; 	// 由输入地址取得像素值并写进输出地址
		
	// 写入像素类型2 (源自两个像素)
	for (;type_2_counter; -- type_2_counter)
	{
		r = *(ratio ++);	// 取得混合比例
		**type_2_addr_out ++ = ((**type_2_addr_in ++) * r + (**++ type_2_addr_in) * (255 - r)) >> 8; 	// 由输入地址取得像素值混合叠加后写进输出地址	
	}
	
}
