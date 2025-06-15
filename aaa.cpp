 #include "F18.h"

//Make sure that that the gravity situation is updated correctly in the weapons.cpp file
//mathcad the equation in HUD to make sure it's correct
//arthrosis


AAAStreamVertex AAAVertexes[MAX_AAA_STREAM_VERTEXES],*LastAAAVertex,*AAAStreams;
BYTE AAAHitCount;
void RemoveStreamVertex(AAAStreamVertex *walker);
extern float CurrentMaxDistAndSmidge;
extern BOOL SpecialDraw;

AAAStreamVertex *InstantiateAAAShots(AAWeaponInstance *gun,AAAStreamVertex *invert, ANGLE pitch, ANGLE heading, BOOL new_trailing_term)
{
	double h_vel;
	double fang;
	AAAStreamVertex *vertex = &AAAVertexes[0],*returner;
	float bulletvel;
//	GDRadarData *radardat;
	int count;

	//First check to see if we have to instantiate a new vertex
	if (!new_trailing_term)
	{
		if (invert && (invert->Flags & AAA_INUSE))
		{
			if (invert->Gun == gun)
			{
				while(invert && !(invert->Flags & AAA_ANCHORED_VERTEX))
					invert = invert->NextVertex;

				if (invert)
				{
					invert->Heading = heading;
					invert->Pitch = pitch;
					invert->LagLoops = AAA_LAG_LOOPS;
					return invert;

				}
			}
		}
	}


	returner = invert;
	count = 1;
	vertex = LastAAAVertex;
	while((vertex->Flags & AAA_INUSE) && ((vertex != LastAAAVertex) || (count--)))
	{
		if (vertex == &AAAVertexes[MAX_AAA_STREAM_VERTEXES-1])
			vertex = AAAVertexes;
		else
			vertex++;
	}

	if (!(vertex->Flags & AAA_INUSE))
	{
		LastAAAVertex = vertex;
		if (LastAAAVertex == &AAAVertexes[MAX_AAA_STREAM_VERTEXES-1])
			LastAAAVertex = AAAVertexes;
		else
			LastAAAVertex++;

	}
	else
		return NULL;

//	while ((vertex <= LastAAAVertex) && (vertex->Flags & AAA_INUSE))
//		vertex++;

//	if (vertex == &AAAVertexes[MAX_AAA_STREAM_VERTEXES])
//		return NULL;

//	if (vertex > LastAAAVertex)
//		LastAAAVertex = vertex;

	returner = vertex;

	vertex->BirthLoop = GameLoop;
	vertex->Weapon = GetWeaponTypeFromAAWeaponFamily(gun->Type);
	vertex->MaxDuration = (DWORD)GDGetAAADuration(vertex->Weapon);

	if (new_trailing_term)
	{
		vertex->NextVertex = NULL;
		vertex->Flags = AAA_INUSE | AAA_TRAILING_TERM | AAA_ANCHORED_VERTEX;
	}
	else
	{

		vertex->Flags = AAA_INUSE | AAA_LEADING_TERM | AAA_CALC_TS | AAA_NO_T;
		returner = vertex->NextVertex = InstantiateAAAShots(gun,invert,pitch,heading,TRUE);

		if (!returner)
		{
			vertex->Flags = 0;
			return NULL;
		}

		/* ------------------------------------------3/11/98 6:58PM--------------------------------------------
		 * if we're in here, it means we have defined a new stream, and it needs to be linked in to the list
		 * ----------------------------------------------------------------------------------------------------*/
		vertex->NextStream = AAAStreams;
		if (AAAStreams)
			AAAStreams->PrevStream = &vertex->NextStream;
		vertex->PrevStream = &AAAStreams;
		AAAStreams = vertex;

		fang = ANGLE_TO_RADIANS(pitch);

//		vertex->Vel.Y = AAA_BULLET_VELOCITY * sin(fang);
//		h_vel =    AAA_BULLET_VELOCITY * cos(fang);


		bulletvel = GDGetWeaponVelocity(vertex->Weapon) FEET / 50;
		vertex->Vel.Y = bulletvel * sin(fang);
		h_vel =    bulletvel * cos(fang);


		fang = ANGLE_TO_RADIANS(heading);

		vertex->Vel.X = -sin(fang)*h_vel; //the negatives are cuz the h_vel is always positive
		vertex->Vel.Z = -cos(fang)*h_vel;
	}

	vertex->Gun = gun;
	vertex->Pitch = pitch;
	vertex->Heading = heading;
	vertex->Position = gun->Basics.Position;
	vertex->Dt = 0.0f;
	vertex->TicksToT1 = 0;
	vertex->LagLoops = AAA_LAG_LOOPS;

	if (!(AAAHitCount++ & 0x3))
		vertex->Flags |= AAA_DETECT_HIT;


//	radardat = GDGetRadarData((BasicInstance *)gun);
//	if(radardat->lWFlags1 & (GD_W_RANDOM_FIRE))
//	{
//		if((!iDoAllAAA) && (vertex->Flags & AAA_DETECT_HIT))
//		{
//			GDCheckForRandomAAAHit(gun->Basics.Position, vertex->Weapon, NULL, vertex);
//		}

//		vertex->Flags &= ~AAA_DETECT_HIT;
//		vertex->Flags |= WEP_NO_CHECK;

//	}

	return returner;
}

void RemoveStreamVertex(AAAStreamVertex *walker)
{
	walker->Flags &= ~(AAA_INUSE | AAA_NO_HEADER);

}

void MoveAAAStreamVertexes()
{
	double h_vel;
	double fang;
	AAAStreamVertex *walker,*stepper;
	AAAStreamVertex *hit_vertex;
	FPointDouble new_point;
	ANGLE last_heading;
	ANGLE last_pitch;
	DWORD loops_since;
	float bulletvel;

	stepper = AAAStreams;

	while(stepper)
	{
		walker = stepper;
		stepper = walker->NextStream;

		hit_vertex = NULL;
		while(walker)
		{
			loops_since = GameLoop - walker->BirthLoop;
			//if we're not flying, should we be?
			if (walker->Flags & AAA_ANCHORED_VERTEX)
			{
				walker->LagLoops--;
				if (walker->LagLoops < 0)
					walker->LagLoops = 0;

				if (loops_since >= AAA_LOOPS_PER_SEGMENT)
				{
					// reset out birthloop
					walker->BirthLoop = GameLoop;

					// Do we have to set yp a new terminus?
					if ( walker->LagLoops && (walker->NextVertex = InstantiateAAAShots(walker->Gun,walker,walker->Pitch,walker->Heading,TRUE)))
					{
						walker->Flags &= ~(AAA_TRAILING_TERM | AAA_ANCHORED_VERTEX);
						walker->Flags |= AAA_WORKHORSE | AAA_CALC_TS;
						walker->NextVertex->LagLoops = walker->LagLoops;
					}
					else
						walker->Flags &= ~AAA_ANCHORED_VERTEX;

				}
				else
					if ( walker->LagLoops && (loops_since >= AAA_LOOPS_BETWEEN_BULLETS) && ((abs(last_heading - walker->Heading) > AAA_SLEW_GRAIN) || (abs(last_pitch - walker->Pitch) > AAA_SLEW_GRAIN)))
					{
						//If we've moved our aim position a lot, then start a new vertex here.
						walker->BirthLoop = GameLoop;

						if (walker->NextVertex = InstantiateAAAShots(walker->Gun,walker,walker->Pitch,walker->Heading,TRUE))
						{
							walker->Flags &= ~(AAA_TRAILING_TERM | AAA_ANCHORED_VERTEX);
							walker->Flags |= AAA_WORKHORSE | AAA_CALC_TS;
							walker->NextVertex->LagLoops = walker->LagLoops;
						}
						else
							walker->Flags &= ~AAA_ANCHORED_VERTEX;

						//Point us towards where we want to go
						if(abs(last_heading - walker->Heading) > AAA_SLEW_GRAIN)
							if ((short)walker->Heading < (short)last_heading)
								walker->Heading -= AAA_SLEW_GRAIN;
							else
								walker->Heading += AAA_SLEW_GRAIN;
						else
							if ((short)walker->Pitch < (short)last_pitch)
								walker->Pitch -= AAA_SLEW_GRAIN;
							else
								walker->Pitch += AAA_SLEW_GRAIN;
					}

				if (!(walker->Flags & AAA_ANCHORED_VERTEX))
				{

					//get us a sort header
//						GetSortHeader(walker,AAA_OBJECT);

					//Now we set up our velocity for next time
					fang = ANGLE_TO_RADIANS(walker->Pitch);

//						walker->Vel.Y = AAA_BULLET_VELOCITY * sin(fang);
//						h_vel =    AAA_BULLET_VELOCITY * cos(fang);

					bulletvel = GDGetWeaponVelocity(walker->Weapon) FEET / 50;
					walker->Vel.Y = bulletvel * sin(fang);
					h_vel =		bulletvel * cos(fang);


					fang = ANGLE_TO_RADIANS(walker->Heading);

					walker->Vel.X = -sin(fang)*h_vel; //the negatives are cuz the h_vel is always positive
					walker->Vel.Z = -cos(fang)*h_vel;
					walker->Flags |= AAA_NO_T;
				}

				//if we're anchored, there isn't anyone after us except maybe a new trailing term which doesn't need updating anyway!
				if (hit_vertex)
				{
					AAAHitsPlane( hit_vertex );
					hit_vertex = NULL;
				}
				walker = NULL;
			}
			else
			{
				//UNmark as bad to draw
				walker->Flags &= ~AAA_NO_T;

				if (loops_since)
				{
					//move him
					walker->LastPosition = walker->Position;
					walker->Position += walker->Vel;
					walker->Position.Y -= 0.5*(32.0 FEET)/2500.0 * 1/2500.0;
					walker->Vel.Y -= (float)((32.0 FEET)/2500.0);

					if (!(walker->Flags & AAA_TRAILING_TERM))
					{
						//Do we have to figure out a new T? (i.e. is the next guy anchored?
						if (walker->Flags & AAA_CALC_TS)
						{
							if (!(walker->NextVertex->Flags & AAA_ANCHORED_VERTEX))
								walker->Flags &= ~AAA_CALC_TS;  //Last time was the last time
							else
							{
								last_heading = walker->Heading;
								last_pitch = walker->Pitch;

							 	walker->NextVertex->TicksToT1 = (walker->TicksToT1 + loops_since) % AAA_LOOPS_BETWEEN_BULLETS;

								//Get the T between bullets
								walker->Dt = ((float)AAA_LOOPS_BETWEEN_BULLETS) / (float)loops_since;

								//and where to start the first bullet
								walker->T1 = (float)walker->TicksToT1 * walker->Dt * (1.0/(double)AAA_LOOPS_BETWEEN_BULLETS);
							}
						}

//							if ((walker->Flags & AAA_LEADING_TERM) && (loops_since >= AAA_STREAM_DURATION)) //2.5 seconds
						if ((walker->Flags & AAA_LEADING_TERM) && (loops_since >= walker->MaxDuration)) //2.5 seconds
						{
							walker->T1 += walker->Dt;
							if ((walker->T1 >= 1.0f) || (!walker->Dt))
							{
								AAAStreamVertex *next_walker;
								next_walker = walker->NextVertex;

								if (next_walker->Flags & AAA_WORKHORSE)
								{
									next_walker->Flags &= ~AAA_WORKHORSE;
									next_walker->Flags |= AAA_LEADING_TERM;

									/* ------------------------------------3/11/98 8:45PM--------------------------------------
									 * set the next vertex with the information about this stream
									 * ----------------------------------------------------------------------------------------*/
									next_walker->PrevStream = walker->PrevStream;
									next_walker->NextStream = walker->NextStream;
									if (walker->NextStream)
										walker->NextStream->PrevStream = &next_walker->NextStream;
									*next_walker->PrevStream = next_walker;
								}
								else
								{
									/* ------------------------------------3/11/98 8:45PM--------------------------------------
									 * this stream is ending, so link the previous stream with the next stream
									 * ----------------------------------------------------------------------------------------*/
									*walker->PrevStream = walker->NextStream;
									if (walker->NextStream)
										walker->NextStream->PrevStream = walker->PrevStream;

									RemoveStreamVertex(next_walker);
									next_walker = NULL;
								}

								RemoveStreamVertex(walker);

								walker = next_walker;
								continue;
							}
							else
								walker->BirthLoop += AAA_LOOPS_BETWEEN_BULLETS;

						}

						if (hit_vertex)
						{
							AAAHitsPlane( hit_vertex );
							hit_vertex = NULL;
						}

						if (walker->Flags & AAA_DETECT_HIT)
							hit_vertex = walker;
					}
				}
				else
					walker->Flags |= AAA_NO_T;

				walker = walker->NextVertex;
			}
		}
	}
}

void DisplayAAAStreams(CameraInstance *camera)
{
	AAAStreamVertex *walker,*vwalker;
	FPoint vector;
	FPoint rel;

	vwalker = AAAStreams;

	while(vwalker)
	{
		walker = vwalker;
		vwalker = vwalker->NextStream;
		while(walker)
		{
			if (((walker->Flags & (AAA_INUSE | AAA_NO_T | AAA_TRAILING_TERM | AAA_NO_HEADER)) == AAA_INUSE) && walker->Dt)
			{
				if (CloseEnoughToDraw(walker->Position))
				{
					rel.MakeVectorToFrom(walker->Position,camera->CameraLocation);
					vector.MakeVectorToFrom(walker->Position,walker->NextVertex->Position);
					if (RadiusInView(rel,vector.QuickLength()))
						RenderAAASegment(walker,rel);
				}
				else
					if (!SpecialDraw)
						walker->Flags |= AAA_NO_HEADER;
			}
			walker = walker->NextVertex;
		}
	}
}

void RenderAAASegment(AAAStreamVertex *from_vertex,FPoint &rel)
{
//	RotPoint3D Points[2];
	//RotPoint3D *points[2];
	//DWORD clip_flags,draw_flags;
	FPointDouble vel,dvel;
	FPoint location,dlocation;
	double t,dt;

	AAAStreamVertex *to_vertex = from_vertex->NextVertex;


//	if (from_vertex->SortHeader->Distance > (LP_HIGH_MIN_DISTANCE * 0.25))
//	{
//		points[0] = &Points[0];
//		points[1] = &Points[1];

//		Points[0].Rotated.RotateInto(from_vertex->SortHeader->RelativePosition,ViewMatrix);

//		if (Points[0].Rotated.Z > ZClipPlane)
//			Points[0].Flags = PT3_RZHIGH | PT3_ROTATED;
//		else
//		{
//			Points[0].Flags = PT3_ROTATED;
//			PerspectPoint(points[0]);
//		}

//		clip_flags = Points[0].Flags;
//		draw_flags = Points[0].Flags;

//		vel = to_vertex->Position;
//		vel -= from_vertex->Position;

//		Points[1].Rotated = from_vertex->SortHeader->RelativePosition;
//		Points[1].Rotated += vel;

//		Points[1].Rotated *= ViewMatrix;

//		if (Points[1].Rotated.Z > ZClipPlane)
//			Points[1].Flags = PT3_RZHIGH | PT3_ROTATED;
//		else
//		{
//			Points[1].Flags = PT3_ROTATED;
//			PerspectPoint(points[1]);
//		}

//		clip_flags |= Points[1].Flags;
//		draw_flags &= Points[1].Flags;

//		if (!(draw_flags & PT3_NODRAW))
//		{
//			Points[0].Light = (float)0x49 * LIGHT_MULTER;
//			Points[1].Light = (float)0x47 * LIGHT_MULTER;

//			clip_flags &= PT3_CLIP_FLAGS;
//			DrawPolyClipped(PRIMITIVE_BLEND_FLAT,CL_CLIP_LIGHT,2,(RotPoint3D **)&points[0],0,clip_flags & PT3_CLIP_FLAGS);
//		}

//	}
//	else
	{
		vel.SetValues(3.0,from_vertex->Vel);

		if (to_vertex->Flags & AAA_ANCHORED_VERTEX)
			dvel.SetValues(0.0);
		else
		{
			dvel.MakeVectorToFrom(to_vertex->Vel,from_vertex->Vel);
			dvel *= 3.0;
		}

		location = rel;
		dlocation.MakeVectorToFrom(to_vertex->Position,from_vertex->Position);

		t = from_vertex->T1;
		dt = from_vertex->Dt;


	//	vel.MakeVectorToFrom(from_vertex->Position,to_vertex->Position);

	//	location.AddScaledVector(t,dlocation);
	//	vel *= 1.0 - t;

	//	RenderBullet(location,vel,location.QuickLength(),lightening,TRUE);

		vel.AddScaledVector(t,dvel);
		location.AddScaledVector(t,dlocation);

		dvel *= dt;
		dlocation *= dt;

		while(t < 1.0f)
		{
			RenderBullet(location,vel,TRUE);
			vel += dvel;
			location += dlocation;
			t += dt;
		}
	}

}