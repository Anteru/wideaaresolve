#if MSAA_SAMPLES == 1
float4 Lanczos2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -2, -2
	// -1, -2
	// 0, -2
	// 1, -2
	// 2, -2
	// -2, -1
	// -1, -1
	// 0, -1
	// 1, -1
	// 2, -1
	// -2, 0
	// -1, 0
	// 0, 0
	result += 1 * source.Load (int3(pos,0), int2(0, 0));
	// 1, 0
	// 2, 0
	// -2, 1
	// -1, 1
	// 0, 1
	// 1, 1
	// 2, 1
	// -2, 2
	// -1, 2
	// 0, 2
	// 1, 2
	// 2, 2

	return result / 1.0; // 1 (25) samples
}

#endif


#if MSAA_SAMPLES == 1
float4 Gaussian2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -1, -1
	result += 0.011660097860112779 * source.Load (int3(pos,0), int2(-1, -1));
	// 0, -1
	result += 0.08615711720739454 * source.Load (int3(pos,0), int2(0, -1));
	// 1, -1
	result += 0.011660097860112779 * source.Load (int3(pos,0), int2(1, -1));
	// -1, 0
	result += 0.08615711720739454 * source.Load (int3(pos,0), int2(-1, 0));
	// 0, 0
	result += 0.6366197723675814 * source.Load (int3(pos,0), int2(0, 0));
	// 1, 0
	result += 0.08615711720739454 * source.Load (int3(pos,0), int2(1, 0));
	// -1, 1
	result += 0.011660097860112779 * source.Load (int3(pos,0), int2(-1, 1));
	// 0, 1
	result += 0.08615711720739454 * source.Load (int3(pos,0), int2(0, 1));
	// 1, 1
	result += 0.011660097860112779 * source.Load (int3(pos,0), int2(1, 1));

	return result / 1.0278886326376107; // 9 (9) samples
}

#endif


#if MSAA_SAMPLES == 1
float4 Box2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// 0, 0
	result += 1.0 * source.Load (int3(pos,0), int2(0, 0));

	return result / 1.0; // 1 (1) samples
}

#endif


#if MSAA_SAMPLES == 2
float4 Lanczos2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -2, -2
	// -1, -2
	result += -0.004213925829850303 * source.Load (pos, 0, int2(-1, -2));
	// 0, -2
	result += -0.015709187066024654 * source.Load (pos, 0, int2(0, -2));
	// 1, -2
	result += 0.001517013298746109 * source.Load (pos, 0, int2(1, -2));
	// 2, -2
	// -2, -1
	result += -0.004213925829850303 * source.Load (pos, 0, int2(-2, -1));
	// -1, -1
	result += 0.055388058619298436 * source.Load (pos, 0, int2(-1, -1));
	result += 0.007178292397061075 * source.Load (pos, 1, int2(-1, -1));
	// 0, -1
	result += 0.20648236566266473 * source.Load (pos, 0, int2(0, -1));
	result += -0.07433365163855929 * source.Load (pos, 1, int2(0, -1));
	// 1, -1
	result += -0.019939701102947432 * source.Load (pos, 0, int2(1, -1));
	result += -0.019939701102947432 * source.Load (pos, 1, int2(1, -1));
	// 2, -1
	result += 0.001517013298746109 * source.Load (pos, 1, int2(2, -1));
	// -2, 0
	result += -0.015709187066024654 * source.Load (pos, 0, int2(-2, 0));
	// -1, 0
	result += 0.20648236566266473 * source.Load (pos, 0, int2(-1, 0));
	result += -0.07433365163855929 * source.Load (pos, 1, int2(-1, 0));
	// 0, 0
	result += 0.7697501662352075 * source.Load (pos, 0, int2(0, 0));
	result += 0.7697501662352075 * source.Load (pos, 1, int2(0, 0));
	// 1, 0
	result += -0.07433365163855929 * source.Load (pos, 0, int2(1, 0));
	result += 0.20648236566266473 * source.Load (pos, 1, int2(1, 0));
	// 2, 0
	result += -0.015709187066024654 * source.Load (pos, 1, int2(2, 0));
	// -2, 1
	result += 0.001517013298746109 * source.Load (pos, 0, int2(-2, 1));
	// -1, 1
	result += -0.019939701102947432 * source.Load (pos, 0, int2(-1, 1));
	result += -0.019939701102947432 * source.Load (pos, 1, int2(-1, 1));
	// 0, 1
	result += -0.07433365163855929 * source.Load (pos, 0, int2(0, 1));
	result += 0.20648236566266473 * source.Load (pos, 1, int2(0, 1));
	// 1, 1
	result += 0.007178292397061075 * source.Load (pos, 0, int2(1, 1));
	result += 0.055388058619298436 * source.Load (pos, 1, int2(1, 1));
	// 2, 1
	result += -0.004213925829850303 * source.Load (pos, 1, int2(2, 1));
	// -2, 2
	// -1, 2
	result += 0.001517013298746109 * source.Load (pos, 1, int2(-1, 2));
	// 0, 2
	result += -0.015709187066024654 * source.Load (pos, 1, int2(0, 2));
	// 1, 2
	result += -0.004213925829850303 * source.Load (pos, 1, int2(1, 2));
	// 2, 2

	return result / 2.0398446877992513; // 30 (50) samples
}

#endif


#if MSAA_SAMPLES == 2
float4 Gaussian2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -1, -1
	result += 0.06709923034829368 * source.Load (pos, 0, int2(-1, -1));
	result += 0.0012289652727713405 * source.Load (pos, 1, int2(-1, -1));
	// 0, -1
	result += 0.1823946185593544 * source.Load (pos, 0, int2(0, -1));
	result += 0.024684427363564163 * source.Load (pos, 1, int2(0, -1));
	// 1, -1
	result += 0.009080893344145044 * source.Load (pos, 0, int2(1, -1));
	result += 0.009080893344145044 * source.Load (pos, 1, int2(1, -1));
	// -1, 0
	result += 0.1823946185593544 * source.Load (pos, 0, int2(-1, 0));
	result += 0.024684427363564163 * source.Load (pos, 1, int2(-1, 0));
	// 0, 0
	result += 0.495799977238612 * source.Load (pos, 0, int2(0, 0));
	result += 0.495799977238612 * source.Load (pos, 1, int2(0, 0));
	// 1, 0
	result += 0.024684427363564163 * source.Load (pos, 0, int2(1, 0));
	result += 0.1823946185593544 * source.Load (pos, 1, int2(1, 0));
	// -1, 1
	result += 0.009080893344145044 * source.Load (pos, 0, int2(-1, 1));
	result += 0.009080893344145044 * source.Load (pos, 1, int2(-1, 1));
	// 0, 1
	result += 0.024684427363564163 * source.Load (pos, 0, int2(0, 1));
	result += 0.1823946185593544 * source.Load (pos, 1, int2(0, 1));
	// 1, 1
	result += 0.0012289652727713405 * source.Load (pos, 0, int2(1, 1));
	result += 0.06709923034829368 * source.Load (pos, 1, int2(1, 1));

	return result / 1.9928961027876086; // 18 (18) samples
}

#endif


#if MSAA_SAMPLES == 2
float4 Box2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// 0, 0
	result += 1.0 * source.Load (pos, 0, int2(0, 0));
	result += 1.0 * source.Load (pos, 1, int2(0, 0));

	return result / 2.0; // 2 (2) samples
}

#endif


#if MSAA_SAMPLES == 4
float4 Lanczos2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -2, -2
	// -1, -2
	result += -0.003912961530890402 * source.Load (pos, 3, int2(-1, -2));
	// 0, -2
	result += -0.0031829155914479537 * source.Load (pos, 2, int2(0, -2));
	result += -0.03813848563717208 * source.Load (pos, 3, int2(0, -2));
	// 1, -2
	result += -0.0017148851338459825 * source.Load (pos, 2, int2(1, -2));
	result += 0.0023671001853534514 * source.Load (pos, 3, int2(1, -2));
	// 2, -2
	// -2, -1
	result += 0.0023671001853534514 * source.Load (pos, 1, int2(-2, -1));
	result += -0.0017148851338459825 * source.Load (pos, 3, int2(-2, -1));
	// -1, -1
	result += 0.0049479509629496085 * source.Load (pos, 0, int2(-1, -1));
	result += -0.0239480826606761 * source.Load (pos, 1, int2(-1, -1));
	result += -0.008179265877528952 * source.Load (pos, 2, int2(-1, -1));
	result += 0.039587646847240116 * source.Load (pos, 3, int2(-1, -1));
	// 0, -1
	result += -0.07972089981722 * source.Load (pos, 0, int2(0, -1));
	result += -0.044448881258303714 * source.Load (pos, 1, int2(0, -1));
	result += 0.07347672208005314 * source.Load (pos, 2, int2(0, -1));
	result += 0.38584915511534473 * source.Load (pos, 3, int2(0, -1));
	// 1, -1
	result += -0.008179265877528952 * source.Load (pos, 0, int2(1, -1));
	result += 0.0049479509629496085 * source.Load (pos, 1, int2(1, -1));
	result += 0.039587646847240116 * source.Load (pos, 2, int2(1, -1));
	result += -0.0239480826606761 * source.Load (pos, 3, int2(1, -1));
	// 2, -1
	result += -0.003912961530890402 * source.Load (pos, 2, int2(2, -1));
	// -2, 0
	result += -0.03813848563717208 * source.Load (pos, 1, int2(-2, 0));
	result += -0.0031829155914479537 * source.Load (pos, 3, int2(-2, 0));
	// -1, 0
	result += -0.044448881258303714 * source.Load (pos, 0, int2(-1, 0));
	result += 0.38584915511534473 * source.Load (pos, 1, int2(-1, 0));
	result += -0.07972089981722 * source.Load (pos, 2, int2(-1, 0));
	result += 0.07347672208005314 * source.Load (pos, 3, int2(-1, 0));
	// 0, 0
	result += 0.716156008075787 * source.Load (pos, 0, int2(0, 0));
	result += 0.716156008075787 * source.Load (pos, 1, int2(0, 0));
	result += 0.716156008075787 * source.Load (pos, 2, int2(0, 0));
	result += 0.716156008075787 * source.Load (pos, 3, int2(0, 0));
	// 1, 0
	result += 0.07347672208005314 * source.Load (pos, 0, int2(1, 0));
	result += -0.07972089981722 * source.Load (pos, 1, int2(1, 0));
	result += 0.38584915511534473 * source.Load (pos, 2, int2(1, 0));
	result += -0.044448881258303714 * source.Load (pos, 3, int2(1, 0));
	// 2, 0
	result += -0.0031829155914479537 * source.Load (pos, 0, int2(2, 0));
	result += -0.03813848563717208 * source.Load (pos, 2, int2(2, 0));
	// -2, 1
	result += -0.003912961530890402 * source.Load (pos, 1, int2(-2, 1));
	// -1, 1
	result += -0.0239480826606761 * source.Load (pos, 0, int2(-1, 1));
	result += 0.039587646847240116 * source.Load (pos, 1, int2(-1, 1));
	result += 0.0049479509629496085 * source.Load (pos, 2, int2(-1, 1));
	result += -0.008179265877528952 * source.Load (pos, 3, int2(-1, 1));
	// 0, 1
	result += 0.38584915511534473 * source.Load (pos, 0, int2(0, 1));
	result += 0.07347672208005314 * source.Load (pos, 1, int2(0, 1));
	result += -0.044448881258303714 * source.Load (pos, 2, int2(0, 1));
	result += -0.07972089981722 * source.Load (pos, 3, int2(0, 1));
	// 1, 1
	result += 0.039587646847240116 * source.Load (pos, 0, int2(1, 1));
	result += -0.008179265877528952 * source.Load (pos, 1, int2(1, 1));
	result += -0.0239480826606761 * source.Load (pos, 2, int2(1, 1));
	result += 0.0049479509629496085 * source.Load (pos, 3, int2(1, 1));
	// 2, 1
	result += -0.0017148851338459825 * source.Load (pos, 0, int2(2, 1));
	result += 0.0023671001853534514 * source.Load (pos, 2, int2(2, 1));
	// -2, 2
	// -1, 2
	result += 0.0023671001853534514 * source.Load (pos, 0, int2(-1, 2));
	result += -0.0017148851338459825 * source.Load (pos, 1, int2(-1, 2));
	// 0, 2
	result += -0.03813848563717208 * source.Load (pos, 0, int2(0, 2));
	result += -0.0031829155914479537 * source.Load (pos, 1, int2(0, 2));
	// 1, 2
	result += -0.003912961530890402 * source.Load (pos, 0, int2(1, 2));
	// 2, 2

	return result / 4.076552823038572; // 56 (100) samples
}

#endif


#if MSAA_SAMPLES == 4
float4 Gaussian2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -1, -1
	result += 0.0011545060309855234 * source.Load (pos, 0, int2(-1, -1));
	result += 0.023188873513402576 * source.Load (pos, 1, int2(-1, -1));
	result += 0.003138272764874324 * source.Load (pos, 2, int2(-1, -1));
	result += 0.0630338934939175 * source.Load (pos, 3, int2(-1, -1));
	// 0, -1
	result += 0.014064762750076879 * source.Load (pos, 0, int2(0, -1));
	result += 0.03823198900512165 * source.Load (pos, 1, int2(0, -1));
	result += 0.10392532097846818 * source.Load (pos, 2, int2(0, -1));
	result += 0.2824983115325437 * source.Load (pos, 3, int2(0, -1));
	// 1, -1
	result += 0.003138272764874324 * source.Load (pos, 0, int2(1, -1));
	result += 0.0011545060309855234 * source.Load (pos, 1, int2(1, -1));
	result += 0.0630338934939175 * source.Load (pos, 2, int2(1, -1));
	result += 0.023188873513402576 * source.Load (pos, 3, int2(1, -1));
	// -1, 0
	result += 0.03823198900512165 * source.Load (pos, 0, int2(-1, 0));
	result += 0.2824983115325437 * source.Load (pos, 1, int2(-1, 0));
	result += 0.014064762750076879 * source.Load (pos, 2, int2(-1, 0));
	result += 0.10392532097846818 * source.Load (pos, 3, int2(-1, 0));
	// 0, 0
	result += 0.4657609751605761 * source.Load (pos, 0, int2(0, 0));
	result += 0.4657609751605761 * source.Load (pos, 1, int2(0, 0));
	result += 0.4657609751605761 * source.Load (pos, 2, int2(0, 0));
	result += 0.4657609751605761 * source.Load (pos, 3, int2(0, 0));
	// 1, 0
	result += 0.10392532097846818 * source.Load (pos, 0, int2(1, 0));
	result += 0.014064762750076879 * source.Load (pos, 1, int2(1, 0));
	result += 0.2824983115325437 * source.Load (pos, 2, int2(1, 0));
	result += 0.03823198900512165 * source.Load (pos, 3, int2(1, 0));
	// -1, 1
	result += 0.023188873513402576 * source.Load (pos, 0, int2(-1, 1));
	result += 0.0630338934939175 * source.Load (pos, 1, int2(-1, 1));
	result += 0.0011545060309855234 * source.Load (pos, 2, int2(-1, 1));
	result += 0.003138272764874324 * source.Load (pos, 3, int2(-1, 1));
	// 0, 1
	result += 0.2824983115325437 * source.Load (pos, 0, int2(0, 1));
	result += 0.10392532097846818 * source.Load (pos, 1, int2(0, 1));
	result += 0.03823198900512165 * source.Load (pos, 2, int2(0, 1));
	result += 0.014064762750076879 * source.Load (pos, 3, int2(0, 1));
	// 1, 1
	result += 0.0630338934939175 * source.Load (pos, 0, int2(1, 1));
	result += 0.003138272764874324 * source.Load (pos, 1, int2(1, 1));
	result += 0.023188873513402576 * source.Load (pos, 2, int2(1, 1));
	result += 0.0011545060309855234 * source.Load (pos, 3, int2(1, 1));

	return result / 3.979987620919865; // 36 (36) samples
}

#endif


#if MSAA_SAMPLES == 4
float4 Box2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// 0, 0
	result += 1.0 * source.Load (pos, 0, int2(0, 0));
	result += 1.0 * source.Load (pos, 1, int2(0, 0));
	result += 1.0 * source.Load (pos, 2, int2(0, 0));
	result += 1.0 * source.Load (pos, 3, int2(0, 0));

	return result / 4.0; // 4 (4) samples
}

#endif


#if MSAA_SAMPLES == 8
float4 Lanczos2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -2, -2
	// -1, -2
	result += 0.002405932633262372 * source.Load (pos, 4, int2(-1, -2));
	result += -0.008428107273678227 * source.Load (pos, 6, int2(-1, -2));
	// 0, -2
	result += -0.009868366477658428 * source.Load (pos, 1, int2(0, -2));
	result += -0.022685000924333287 * source.Load (pos, 4, int2(0, -2));
	result += -0.04800798755601872 * source.Load (pos, 6, int2(0, -2));
	// 1, -2
	result += -0.008768729679906667 * source.Load (pos, 4, int2(1, -2));
	result += 0.00394556822507374 * source.Load (pos, 6, int2(1, -2));
	// 2, -2
	// -2, -1
	result += -0.0012485408781958368 * source.Load (pos, 2, int2(-2, -1));
	result += -0.004830385626683885 * source.Load (pos, 6, int2(-2, -1));
	result += 0.0038396935492568773 * source.Load (pos, 7, int2(-2, -1));
	// -1, -1
	result += -0.0034199044182045436 * source.Load (pos, 0, int2(-1, -1));
	result += -0.005687470288664516 * source.Load (pos, 1, int2(-1, -1));
	result += 0.014073047916500402 * source.Load (pos, 2, int2(-1, -1));
	result += 0.006590140367919844 * source.Load (pos, 3, int2(-1, -1));
	result += -0.02711869977433171 * source.Load (pos, 4, int2(-1, -1));
	result += 0.0025911088065024837 * source.Load (pos, 5, int2(-1, -1));
	result += 0.07924134588742737 * source.Load (pos, 6, int2(-1, -1));
	result += -0.0361009269054546 * source.Load (pos, 7, int2(-1, -1));
	// 0, -1
	result += -0.07578701673667451 * source.Load (pos, 0, int2(0, -1));
	result += 0.16188824285171302 * source.Load (pos, 1, int2(0, -1));
	result += 0.03640745200819057 * source.Load (pos, 2, int2(0, -1));
	result += -0.08018601091851595 * source.Load (pos, 3, int2(0, -1));
	result += 0.2556961574660782 * source.Load (pos, 4, int2(0, -1));
	result += -0.022957175394643436 * source.Load (pos, 5, int2(0, -1));
	result += 0.45137270133790514 * source.Load (pos, 6, int2(0, -1));
	result += -0.04897568302623569 * source.Load (pos, 7, int2(0, -1));
	// 1, -1
	result += 0.002662555342892806 * source.Load (pos, 0, int2(1, -1));
	result += 0.00730523961521799 * source.Load (pos, 1, int2(1, -1));
	result += -0.0038613124668855957 * source.Load (pos, 2, int2(1, -1));
	result += -0.014077163744491505 * source.Load (pos, 3, int2(1, -1));
	result += 0.09883757521058091 * source.Load (pos, 4, int2(1, -1));
	result += -0.016922179736293997 * source.Load (pos, 5, int2(1, -1));
	result += -0.03709636414120561 * source.Load (pos, 6, int2(1, -1));
	result += 0.005527741170778493 * source.Load (pos, 7, int2(1, -1));
	// 2, -1
	result += -0.008768729679906667 * source.Load (pos, 4, int2(2, -1));
	result += 0.001799842550940045 * source.Load (pos, 5, int2(2, -1));
	// -2, 0
	result += -0.027668372229516153 * source.Load (pos, 2, int2(-2, 0));
	result += -0.006553057099240588 * source.Load (pos, 6, int2(-2, 0));
	result += -0.034019612781508494 * source.Load (pos, 7, int2(-2, 0));
	// -1, 0
	result += 0.041611935058826335 * source.Load (pos, 0, int2(-1, 0));
	result += -0.03239683525341145 * source.Load (pos, 1, int2(-1, 0));
	result += 0.3118667037319661 * source.Load (pos, 2, int2(-1, 0));
	result += -0.06213696022528856 * source.Load (pos, 3, int2(-1, 0));
	result += -0.07015699558593072 * source.Load (pos, 4, int2(-1, 0));
	result += -0.07375336141242204 * source.Load (pos, 5, int2(-1, 0));
	result += 0.1075013682867121 * source.Load (pos, 6, int2(-1, 0));
	result += 0.3198535348256627 * source.Load (pos, 7, int2(-1, 0));
	// 0, 0
	result += 0.9221440230789707 * source.Load (pos, 0, int2(0, 0));
	result += 0.9221440230789707 * source.Load (pos, 1, int2(0, 0));
	result += 0.8068097342126904 * source.Load (pos, 2, int2(0, 0));
	result += 0.7560559704195031 * source.Load (pos, 3, int2(0, 0));
	result += 0.6614946269535582 * source.Load (pos, 4, int2(0, 0));
	result += 0.6534533978814128 * source.Load (pos, 5, int2(0, 0));
	result += 0.6123467800512593 * source.Load (pos, 6, int2(0, 0));
	result += 0.4339236324172002 * source.Load (pos, 7, int2(0, 0));
	// 1, 0
	result += -0.03239683525341145 * source.Load (pos, 0, int2(1, 0));
	result += 0.041611935058826335 * source.Load (pos, 1, int2(1, 0));
	result += -0.08556886882441697 * source.Load (pos, 2, int2(1, 0));
	result += 0.13273042983035013 * source.Load (pos, 3, int2(1, 0));
	result += 0.2556961574660782 * source.Load (pos, 4, int2(1, 0));
	result += 0.48167318749594146 * source.Load (pos, 5, int2(1, 0));
	result += -0.05032612531981812 * source.Load (pos, 6, int2(1, 0));
	result += -0.04897568302623569 * source.Load (pos, 7, int2(1, 0));
	// 2, 0
	result += -0.008090967578805624 * source.Load (pos, 3, int2(2, 0));
	result += -0.022685000924333287 * source.Load (pos, 4, int2(2, 0));
	result += -0.05123074639390275 * source.Load (pos, 5, int2(2, 0));
	// -2, 1
	result += -0.02507651713033195 * source.Load (pos, 7, int2(-2, 1));
	// -1, 1
	result += 0.00730523961521799 * source.Load (pos, 0, int2(-1, 1));
	result += 0.002662555342892806 * source.Load (pos, 1, int2(-1, 1));
	result += -0.010956525194507216 * source.Load (pos, 2, int2(-1, 1));
	result += -0.02401861076241862 * source.Load (pos, 3, int2(-1, 1));
	result += 0.007440731684113685 * source.Load (pos, 4, int2(-1, 1));
	result += -0.003328135311463196 * source.Load (pos, 5, int2(-1, 1));
	result += -0.012133362980872624 * source.Load (pos, 6, int2(-1, 1));
	result += 0.23577025102451207 * source.Load (pos, 7, int2(-1, 1));
	// 0, 1
	result += 0.16188824285171302 * source.Load (pos, 0, int2(0, 1));
	result += -0.07578701673667451 * source.Load (pos, 1, int2(0, 1));
	result += -0.028344902082501264 * source.Load (pos, 2, int2(0, 1));
	result += 0.29224818855426077 * source.Load (pos, 3, int2(0, 1));
	result += -0.07015699558593072 * source.Load (pos, 4, int2(0, 1));
	result += 0.029487216395786504 * source.Load (pos, 5, int2(0, 1));
	result += -0.0691137784657284 * source.Load (pos, 6, int2(0, 1));
	result += 0.3198535348256627 * source.Load (pos, 7, int2(0, 1));
	// 1, 1
	result += -0.005687470288664516 * source.Load (pos, 0, int2(1, 1));
	result += -0.0034199044182045436 * source.Load (pos, 1, int2(1, 1));
	result += 0.0030062121281981234 * source.Load (pos, 2, int2(1, 1));
	result += 0.05130602653984096 * source.Load (pos, 3, int2(1, 1));
	result += -0.02711869977433171 * source.Load (pos, 4, int2(1, 1));
	result += 0.021735599750173213 * source.Load (pos, 5, int2(1, 1));
	result += 0.0056801616170844135 * source.Load (pos, 6, int2(1, 1));
	result += -0.0361009269054546 * source.Load (pos, 7, int2(1, 1));
	// 2, 1
	result += -0.0031275073685949435 * source.Load (pos, 3, int2(2, 1));
	result += 0.002405932633262372 * source.Load (pos, 4, int2(2, 1));
	result += -0.0023117977654296615 * source.Load (pos, 5, int2(2, 1));
	// -2, 2
	result += 0.002667137642070265 * source.Load (pos, 7, int2(-2, 2));
	// -1, 2
	result += 0.0021308971270675107 * source.Load (pos, 3, int2(-1, 2));
	result += -0.02507651713033195 * source.Load (pos, 7, int2(-1, 2));
	// 0, 2
	result += -0.009868366477658428 * source.Load (pos, 0, int2(0, 2));
	result += -0.02592784535046309 * source.Load (pos, 3, int2(0, 2));
	result += -0.034019612781508494 * source.Load (pos, 7, int2(0, 2));
	// 1, 2
	result += -0.004551798005156044 * source.Load (pos, 3, int2(1, 2));
	result += 0.0038396935492568773 * source.Load (pos, 7, int2(1, 2));
	// 2, 2

	return result / 8.157647937996831; // 102 (200) samples
}

#endif


#if MSAA_SAMPLES == 8
float4 Gaussian2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// -1, -1
	result += 0.006540722109644869 * source.Load (pos, 0, int2(-1, -1));
	result += 0.01777952605564796 * source.Load (pos, 1, int2(-1, -1));
	result += 0.042650865793338176 * source.Load (pos, 2, int2(-1, -1));
	result += 0.0012099119322467645 * source.Load (pos, 3, int2(-1, -1));
	result += 0.007889616861710254 * source.Load (pos, 4, int2(-1, -1));
	result += 0.0010677435326079125 * source.Load (pos, 5, int2(-1, -1));
	result += 0.09029188359304187 * source.Load (pos, 6, int2(-1, -1));
	result += 0.0054224490828219335 * source.Load (pos, 7, int2(-1, -1));
	// 0, -1
	result += 0.03763925695517221 * source.Load (pos, 0, int2(0, -1));
	result += 0.16868744651164128 * source.Load (pos, 1, int2(0, -1));
	result += 0.09029188359304187 * source.Load (pos, 2, int2(0, -1));
	result += 0.018926206968422957 * source.Load (pos, 3, int2(0, -1));
	result += 0.2034759006799254 * source.Load (pos, 4, int2(0, -1));
	result += 0.04540161030505776 * source.Load (pos, 5, int2(0, -1));
	result += 0.31514964001493806 * source.Load (pos, 6, int2(0, -1));
	result += 0.006962562443038495 * source.Load (pos, 7, int2(0, -1));
	// 1, -1
	result += 0.00396714849615991 * source.Load (pos, 0, int2(1, -1));
	result += 0.029313482790913937 * source.Load (pos, 1, int2(1, -1));
	result += 0.0035009962599542557 * source.Load (pos, 2, int2(1, -1));
	result += 0.0054224490828219335 * source.Load (pos, 3, int2(1, -1));
	result += 0.09611520977004946 * source.Load (pos, 4, int2(1, -1));
	result += 0.03535880965828175 * source.Load (pos, 5, int2(1, -1));
	result += 0.020146842446218817 * source.Load (pos, 6, int2(1, -1));
	// -1, 0
	result += 0.10231410821794534 * source.Load (pos, 0, int2(-1, 0));
	result += 0.062056643555340164 * source.Load (pos, 1, int2(-1, 0));
	result += 0.24543878642830513 * source.Load (pos, 2, int2(-1, 0));
	result += 0.03120404000251192 * source.Load (pos, 3, int2(-1, 0));
	result += 0.016702319027308243 * source.Load (pos, 4, int2(-1, 0));
	result += 0.01013046857836414 * source.Load (pos, 5, int2(-1, 0));
	result += 0.11593707345407663 * source.Load (pos, 6, int2(-1, 0));
	result += 0.23056840209183666 * source.Load (pos, 7, int2(-1, 0));
	// 0, 0
	result += 0.5887770409441282 * source.Load (pos, 0, int2(0, 0));
	result += 0.5887770409441282 * source.Load (pos, 1, int2(0, 0));
	result += 0.5195939149461166 * source.Load (pos, 2, int2(0, 0));
	result += 0.4881133110587761 * source.Load (pos, 3, int2(0, 0));
	result += 0.430758485119681 * source.Load (pos, 4, int2(0, 0));
	result += 0.430758485119681 * source.Load (pos, 5, int2(0, 0));
	result += 0.4046601478391726 * source.Load (pos, 6, int2(0, 0));
	result += 0.2960556885709973 * source.Load (pos, 7, int2(0, 0));
	// 1, 0
	result += 0.062056643555340164 * source.Load (pos, 0, int2(1, 0));
	result += 0.10231410821794534 * source.Load (pos, 1, int2(1, 0));
	result += 0.020146842446218814 * source.Load (pos, 2, int2(1, 0));
	result += 0.1398468050296494 * source.Load (pos, 3, int2(1, 0));
	result += 0.2034759006799254 * source.Load (pos, 4, int2(1, 0));
	result += 0.33547504552585966 * source.Load (pos, 5, int2(1, 0));
	result += 0.02586905776694839 * source.Load (pos, 6, int2(1, 0));
	result += 0.006962562443038495 * source.Load (pos, 7, int2(1, 0));
	// -1, 1
	result += 0.029313482790913937 * source.Load (pos, 0, int2(-1, 1));
	result += 0.00396714849615991 * source.Load (pos, 1, int2(-1, 1));
	result += 0.025869057766948395 * source.Load (pos, 2, int2(-1, 1));
	result += 0.014739744807579279 * source.Load (pos, 3, int2(-1, 1));
	result += 0.0017604114738631613 * source.Load (pos, 5, int2(-1, 1));
	result += 0.0027265786287824344 * source.Load (pos, 6, int2(-1, 1));
	result += 0.17956685210064494 * source.Load (pos, 7, int2(-1, 1));
	// 0, 1
	result += 0.16868744651164128 * source.Load (pos, 0, int2(0, 1));
	result += 0.03763925695517221 * source.Load (pos, 1, int2(0, 1));
	result += 0.05476479572238399 * source.Load (pos, 2, int2(0, 1));
	result += 0.23056840209183668 * source.Load (pos, 3, int2(0, 1));
	result += 0.016702319027308243 * source.Load (pos, 4, int2(0, 1));
	result += 0.07485460063398686 * source.Load (pos, 5, int2(0, 1));
	result += 0.009516694514936735 * source.Load (pos, 6, int2(0, 1));
	result += 0.23056840209183666 * source.Load (pos, 7, int2(0, 1));
	// 1, 1
	result += 0.01777952605564796 * source.Load (pos, 0, int2(1, 1));
	result += 0.006540722109644869 * source.Load (pos, 1, int2(1, 1));
	result += 0.002123461571201517 * source.Load (pos, 2, int2(1, 1));
	result += 0.0660589532037003 * source.Load (pos, 3, int2(1, 1));
	result += 0.007889616861710254 * source.Load (pos, 4, int2(1, 1));
	result += 0.05829682159024624 * source.Load (pos, 5, int2(1, 1));
	result += 0.0054224490828219335 * source.Load (pos, 7, int2(1, 1));

	return result / 7.956561830588962; // 69 (72) samples
}

#endif


#if MSAA_SAMPLES == 8
float4 Box2D (int2 pos)
{
	float4 result = float4(0,0,0,0);

	// 0, 0
	result += 1.0 * source.Load (pos, 0, int2(0, 0));
	result += 1.0 * source.Load (pos, 1, int2(0, 0));
	result += 1.0 * source.Load (pos, 2, int2(0, 0));
	result += 1.0 * source.Load (pos, 3, int2(0, 0));
	result += 1.0 * source.Load (pos, 4, int2(0, 0));
	result += 1.0 * source.Load (pos, 5, int2(0, 0));
	result += 1.0 * source.Load (pos, 6, int2(0, 0));
	result += 1.0 * source.Load (pos, 7, int2(0, 0));

	return result / 8.0; // 8 (8) samples
}

#endif


