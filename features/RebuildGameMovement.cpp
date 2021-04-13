#include "RebuildGameMovement.hpp"

#include "../Structs.hpp"
#include "../helpers/Math.hpp"

void RebuildGameMovement::SetAbsOrigin(C_BasePlayer *player, const Vector &vec)
{
	player->SetAbsOrigin(vec);
}

int RebuildGameMovement::ClipVelocity(Vector &in, Vector &normal, Vector &out, float overbounce)
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;

	angle = normal[2];

	blocked = 0x00;         // Assume unblocked.
	if (angle > 0)			// If the plane that is blocking us has a positive z component, then assume it's a floor.
		blocked |= 0x01;	// 
	if (!angle)				// If the plane has no Z, it is vertical (wall/step)
		blocked |= 0x02;	// 

							// Determine how far along plane to slide based on incoming direction.
	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	// iterate once to make sure we aren't still moving through the plane
	float adjust = out.Dot(normal);
	if (adjust < 0.0f)
	{
		out -= (normal * adjust);
		//		Msg( "Adjustment = %lf\n", adjust );
	}

	// Return blocking flags.
	return blocked;
}

int RebuildGameMovement::TryPlayerMove(C_BasePlayer *player, Vector *pFirstDest, trace_t *pFirstTrace)
{
	Vector  planes[5];
	numbumps[player->EntIndex()] = 4;           // Bump up to four times

	blocked[player->EntIndex()] = 0;           // Assume not blocked
	numplanes[player->EntIndex()] = 0;           //  and not sliding along any planes

	original_velocity[player->EntIndex()] = player->m_vecVelocity(); // Store original velocity
	primal_velocity[player->EntIndex()] = player->m_vecVelocity();

	allFraction[player->EntIndex()] = 0;
	time_left[player->EntIndex()] = g_GlobalVars->frametime;   // Total time for this movement operation.

	new_velocity[player->EntIndex()].Zero();

	for (bumpcount[player->EntIndex()] = 0; bumpcount[player->EntIndex()] < numbumps[player->EntIndex()]; bumpcount[player->EntIndex()]++)
	{
		if (player->m_vecVelocity().Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA(player->GetAbsOrigin(), time_left[player->EntIndex()], player->m_vecVelocity(), end[player->EntIndex()]);

		// See if we can make it from origin to end point.
		if (true)
		{
			// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
			if (pFirstDest && end[player->EntIndex()] == *pFirstDest)
				pm[player->EntIndex()] = *pFirstTrace;
			else
			{
				TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
			}
		}
		else
		{
			TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
		}

		allFraction[player->EntIndex()] += pm[player->EntIndex()].fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm[player->EntIndex()].allsolid)
		{
			// C_BasePlayer is trapped in another solid
			player->m_vecVelocity() = vec3_origin[player->EntIndex()];
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if (pm[player->EntIndex()].fraction > 0)
		{
			if (numbumps[player->EntIndex()] > 0 && pm[player->EntIndex()].fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox(pm[player->EntIndex()].endpos, pm[player->EntIndex()].endpos, MASK_PLAYERSOLID, 8, stuck, player);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					player->m_vecVelocity() = vec3_origin[player->EntIndex()];
					break;
				}
			}

			// actually covered some distance
			SetAbsOrigin(player, pm[player->EntIndex()].endpos);
			original_velocity[player->EntIndex()] = player->m_vecVelocity();
			numplanes[player->EntIndex()] = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm[player->EntIndex()].fraction == 1)
		{
			break;		// moved the entire distance
		}

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (pm[player->EntIndex()].plane.normal[2] > 0.7)
		{
			blocked[player->EntIndex()] |= 1;		// floor
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!pm[player->EntIndex()].plane.normal[2])
		{
			blocked[player->EntIndex()] |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left[player->EntIndex()] -= time_left[player->EntIndex()] * pm[player->EntIndex()].fraction;

		// Did we run out of planes to clip against?
		if (numplanes[player->EntIndex()] >= 5)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			player->m_vecVelocity() = vec3_origin[player->EntIndex()];
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		planes[numplanes[player->EntIndex()]] = pm[player->EntIndex()].plane.normal;
		numplanes[player->EntIndex()]++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if (numplanes[player->EntIndex()] == 1 &&
			player->m_fFlags() & FL_ONGROUND)
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{
				if (planes[i[player->EntIndex()]][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1);
					original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
				}
				else
				{
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1.0 + g_CVar->FindVar("sv_bounce")->GetFloat() * (1 - player->m_surfaceFriction()));
				}
			}

			player->m_vecVelocity() = new_velocity[player->EntIndex()];
			original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
		}
		else
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{


				for (j[player->EntIndex()] = 0; j[player->EntIndex()]<numplanes[player->EntIndex()]; j[player->EntIndex()]++)
					if (j[player->EntIndex()] != i[player->EntIndex()])
					{
						// Are we now moving against this plane?
						if (player->m_vecVelocity().Dot(planes[j[player->EntIndex()]]) < 0)
							break;	// not ok
					}
				if (j[player->EntIndex()] == numplanes[player->EntIndex()])  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i[player->EntIndex()] != numplanes[player->EntIndex()])
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;
			}
			else
			{	// go along the crease
				if (numplanes[player->EntIndex()] != 2)
				{
					player->m_vecVelocity() = vec3_origin[player->EntIndex()];
					break;
				}

				dir[player->EntIndex()] = planes[0].Cross(planes[1]);
				dir[player->EntIndex()].NormalizeInPlace();
				d[player->EntIndex()] = dir[player->EntIndex()].Dot(player->m_vecVelocity());
				VectorMultiply(dir[player->EntIndex()], d[player->EntIndex()], player->m_vecVelocity());
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d[player->EntIndex()] = player->m_vecVelocity().Dot(primal_velocity[player->EntIndex()]);
			if (d[player->EntIndex()] <= 0)
			{
				//Con_DPrintf("Back\n");
				player->m_vecVelocity() = vec3_origin[player->EntIndex()];
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		player->m_vecVelocity() = vec3_origin[player->EntIndex()];
	}

	// Check if they slammed into a wall
	float fSlamVol = 0.0f;

	float fLateralStoppingAmount = primal_velocity[player->EntIndex()].Length2D() - player->m_vecVelocity().Length2D();
	if (fLateralStoppingAmount > 580.f * 2.0f)
	{
		fSlamVol = 1.0f;
	}
	else if (fLateralStoppingAmount > 580.f)
	{
		fSlamVol = 0.85f;
	}

	return blocked[player->EntIndex()];
}

void RebuildGameMovement::Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel)
{
	// See if we are changing direction a bit
	currentspeed[player->EntIndex()] = player->m_vecVelocity().Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	addspeed[player->EntIndex()] = wishspeed - currentspeed[player->EntIndex()];

	// If not going to add any speed, done.
	if (addspeed[player->EntIndex()] <= 0)
		return;

	// Determine amount of accleration.
	accelspeed[player->EntIndex()] = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust velocity.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->m_vecVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
	}
}

void RebuildGameMovement::AirAccelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel)
{

	wishspd[player->EntIndex()] = wishspeed;

	// Cap speed
	if (wishspd[player->EntIndex()] > 30.f)
		wishspd[player->EntIndex()] = 30.f;

	// Determine veer amount
	currentspeed[player->EntIndex()] = player->m_vecVelocity().Dot(wishdir);

	// See how much to add
	addspeed[player->EntIndex()] = wishspd[player->EntIndex()] - currentspeed[player->EntIndex()];

	// If not adding any, done.
	if (addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	accelspeed[player->EntIndex()] = accel * wishspeed * g_GlobalVars->frametime * player->m_surfaceFriction();

	// Cap it
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust pmove vel.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->m_vecVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
		g_MoveHelper->SetHost(player);
		g_MoveHelper->m_outWishVel[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];

	}
}

void RebuildGameMovement::AirMove(C_BasePlayer *player)
{
	Math::AngleVectors(player->m_angEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles

																																						 // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove[player->EntIndex()] = g_MoveHelper->m_flForwardMove;
	smove[player->EntIndex()] = g_MoveHelper->m_flSideMove;

	// Zero out z components of movement vectors
	forward[player->EntIndex()][2] = 0;
	right[player->EntIndex()][2] = 0;
	Math::NormalizeVector(forward[player->EntIndex()]);  // Normalize remainder of vectors
	Math::NormalizeVector(right[player->EntIndex()]);    // 

	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize();

	//
	// clamp to server defined max speed
	//
	if (wishspeed != 0 && (wishspeed[player->EntIndex()] > player->m_flMaxspeed()))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}

	AirAccelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], g_CVar->FindVar("sv_airaccelerate")->GetFloat());

	// Add in any base velocity to the current velocity.
	VectorAdd(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
	trace_t trace;
	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
}

void RebuildGameMovement::StepMove(C_BasePlayer *player, Vector &vecDestination, trace_t &trace)
{
	Vector vecEndPos;
	vecEndPos = vecDestination;

	// Try sliding forward both on ground and up 16 pixels
	//  take the move that goes farthest
	Vector vecPos, vecVel;
	vecPos = player->GetAbsOrigin();
	vecVel = player->m_vecVelocity();

	// Slide move down.
	TryPlayerMove(player, &vecEndPos, &trace);

	// Down results.
	Vector vecDownPos, vecDownVel;
	vecDownPos = player->GetAbsOrigin();
	vecDownVel = player->m_vecVelocity();

	// Reset original values.
	SetAbsOrigin(player, vecPos);
	player->m_vecVelocity() = vecVel;

	// Move up a stair height.
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z += player->m_flStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);
	if (!trace.startsolid && !trace.allsolid)
	{
		SetAbsOrigin(player, trace.endpos);
	}

	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Move down a stair (attempt to).
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z -= player->m_flStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);

	// If we are not on the ground any more then use the original movement attempt.
	if (trace.plane.normal[2] < 0.7)
	{
		SetAbsOrigin(player, vecDownPos);
		player->m_vecVelocity() = vecDownVel;

		float flStepDist = player->GetAbsOrigin().z - vecPos.z;
		if (flStepDist > 0.0f)
		{
			g_MoveHelper->SetHost(player);
			g_MoveHelper->m_outStepHeight += flStepDist;
			g_MoveHelper->SetHost(nullptr);
		}
		return;
	}

	// If the trace ended up in empty space, copy the end over to the origin.
	if (!trace.startsolid && !trace.allsolid)
	{
		player->SetAbsOrigin(trace.endpos);
	}

	// Copy this origin to up.
	Vector vecUpPos;
	vecUpPos = player->GetAbsOrigin();

	// decide which one went farther
	float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);
	float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
	if (flDownDist > flUpDist)
	{
		SetAbsOrigin(player, vecDownPos);
		player->m_vecVelocity() = vecDownVel;
	}
	else
	{
		// copy z value from slide move
		player->m_vecVelocity() = vecDownVel;
	}

	float flStepDist = player->GetAbsOrigin().z - vecPos.z;
	if (flStepDist > 0)
	{
		g_MoveHelper->SetHost(player);
		g_MoveHelper->m_outStepHeight += flStepDist;
		g_MoveHelper->SetHost(nullptr);
	}
}

void RebuildGameMovement::TracePlayerBBox(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t& pm, C_BasePlayer *player)
{
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = reinterpret_cast<void*>(player);

	ray.Init(start, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());
	g_EngineTrace->TraceRay(ray, fMask, &filter, &pm);
}

void RebuildGameMovement::WalkMove(C_BasePlayer *player)
{
	Math::AngleVectors(player->m_angEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles
																																						 // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove[player->EntIndex()] = g_MoveHelper->m_flForwardMove;
	smove[player->EntIndex()] = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);


	if (forward[player->EntIndex()][2] != 0)
	{
		forward[player->EntIndex()][2] = 0;
		Math::NormalizeVector(forward[player->EntIndex()]);
	}

	if (right[player->EntIndex()][2] != 0)
	{
		right[player->EntIndex()][2] = 0;
		Math::NormalizeVector(right[player->EntIndex()]);
	}


	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize();

	//
	// Clamp to server defined max speed
	//
	g_MoveHelper->SetHost(player);
	if ((wishspeed[player->EntIndex()] != 0.0f) && (wishspeed[player->EntIndex()] > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	player->m_vecVelocity()[2] = 0;
	Accelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], g_CVar->FindVar("sv_accelerate")->GetFloat());
	player->m_vecVelocity()[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());

	spd[player->EntIndex()] = player->m_vecVelocity().Length();

	if (spd[player->EntIndex()] < 1.0f)
	{
		player->m_vecVelocity().Zero();
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
		return;
	}

	// first try just moving to the destination	
	dest[player->EntIndex()][0] = player->GetAbsOrigin()[0] + player->m_vecVelocity()[0] * g_GlobalVars->frametime;
	dest[player->EntIndex()][1] = player->GetAbsOrigin()[1] + player->m_vecVelocity()[1] * g_GlobalVars->frametime;
	dest[player->EntIndex()][2] = player->GetAbsOrigin()[2];

	// first try moving directly to the next spot
	TracePlayerBBox(player->GetAbsOrigin(), dest[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);

	// If we made it all the way, then copy trace end as new player position.
	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir[player->EntIndex()] * wishspeed[player->EntIndex()];
	g_MoveHelper->SetHost(nullptr);

	if (pm[player->EntIndex()].fraction == 1)
	{
		player->SetAbsOrigin(pm[player->EntIndex()].endpos);
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());

		return;
	}

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
		return;
	}

	StepMove(player, dest[player->EntIndex()], pm[player->EntIndex()]);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
}

void RebuildGameMovement::FinishGravity(C_BasePlayer *player)
{
	float ent_gravity;

	ent_gravity = g_CVar->FindVar("sv_gravity")->GetFloat();

	// Get the correct velocity for the end of the dt 
	player->m_vecVelocity()[2] -= (ent_gravity * g_CVar->FindVar("sv_gravity")->GetFloat() * g_GlobalVars->frametime * 0.5);

	CheckVelocity(player);
}

void RebuildGameMovement::FullWalkMove(C_BasePlayer *player)
{

	StartGravity(player);

	// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
	//  we don't slow when standing still, relative to the conveyor.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		player->m_vecVelocity()[2] = 0.0;
		Friction(player);
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	if (player->m_fFlags() & FL_ONGROUND)
	{
		WalkMove(player);
	}
	else
	{
		AirMove(player);  // Take into account movement when in air.
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	// Add any remaining gravitational component.
	FinishGravity(player);


	// If we are on ground, no downward velocity.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		player->m_vecVelocity()[2] = 0;
	}

	CheckFalling(player);
}

void RebuildGameMovement::Friction(C_BasePlayer *player)
{
	// Calculate speed
	speed[player->EntIndex()] = player->m_vecVelocity().Length();

	// If too slow, return
	if (speed[player->EntIndex()] < 0.1f)
	{
		return;
	}

	drop[player->EntIndex()] = 0;

	// apply ground friction
	if (player->m_fFlags() & FL_ONGROUND)  // On an C_BasePlayer that is the ground
	{
		friction[player->EntIndex()] = g_CVar->FindVar("sv_friction")->GetFloat() * player->m_surfaceFriction();

		//  Bleed off some speed, but if we have less than the bleed
		//  threshold, bleed the threshold amount.


		control[player->EntIndex()] = (speed[player->EntIndex()] < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed[player->EntIndex()];

		// Add the amount to the drop amount.
		drop[player->EntIndex()] += control[player->EntIndex()] * friction[player->EntIndex()] * g_GlobalVars->frametime;
	}

	// scale the velocity
	newspeed[player->EntIndex()] = speed[player->EntIndex()] - drop[player->EntIndex()];
	if (newspeed[player->EntIndex()] < 0)
		newspeed[player->EntIndex()] = 0;

	if (newspeed[player->EntIndex()] != speed[player->EntIndex()])
	{
		// Determine proportion of old speed we are using.
		newspeed[player->EntIndex()] /= speed[player->EntIndex()];
		// Adjust velocity according to proportion.
		VectorMultiply(player->m_vecVelocity(), newspeed[player->EntIndex()], player->m_vecVelocity());
	}

	player->m_vecVelocity() -= (1.f - newspeed[player->EntIndex()]) * player->m_vecVelocity();
}


void RebuildGameMovement::CheckFalling(C_BasePlayer *player)
{
	// this function really deals with landing, not falling, so early out otherwise
	if (player->m_flFallVelocity() <= 0)
		return;

	if (!player->m_iHealth() && player->m_flFallVelocity() >= 303.0f)
	{
		bool bAlive = true;
		float fvol = 0.5;

		//
		// They hit the ground.
		//
		if (player->m_vecVelocity().z < 0.0f)
		{
			// Player landed on a descending object. Subtract the velocity of the ground C_BasePlayer.
			player->m_flFallVelocity() += player->m_vecVelocity().z;
			player->m_flFallVelocity() = max(0.1f, player->m_flFallVelocity());
		}

		if (player->m_flFallVelocity() > 526.5f)
		{
			fvol = 1.0;
		}
		else if (player->m_flFallVelocity() > 526.5f / 2)
		{
			fvol = 0.85;
		}
		else if (player->m_flFallVelocity() < 173)
		{
			fvol = 0;
		}

	}

	// let any subclasses know that the player has landed and how hard

	//
	// Clear the fall velocity so the impact doesn't happen again.
	//
	player->m_flFallVelocity() = 0;
}

const int nanmask = 255 << 23;
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

void RebuildGameMovement::CheckVelocity(C_BasePlayer *player)
{
	Vector org = player->GetAbsOrigin();

	for (i[player->EntIndex()] = 0; i[player->EntIndex()] < 3; i[player->EntIndex()]++)
	{
		// See if it's bogus.
		if (IS_NAN(player->m_vecVelocity()[i[player->EntIndex()]]))
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = 0;
		}

		if (IS_NAN(org[i[player->EntIndex()]]))
		{
			org[i[player->EntIndex()]] = 0;
			player->SetAbsOrigin(org);
		}

		// Bound it.
		if (player->m_vecVelocity()[i[player->EntIndex()]] > g_CVar->FindVar("sv_maxvelocity")->GetFloat())
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = g_CVar->FindVar("sv_maxvelocity")->GetFloat();
		}
		else if (player->m_vecVelocity()[i[player->EntIndex()]] < -g_CVar->FindVar("sv_maxvelocity")->GetFloat())
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = -g_CVar->FindVar("sv_maxvelocity")->GetFloat();
		}
	}
}
void RebuildGameMovement::StartGravity(C_BasePlayer *player)
{
	if (!player || !player->m_iHealth())
		return;

	Vector pVel = player->m_vecVelocity();

	pVel[2] -= (g_CVar->FindVar("sv_gravity")->GetFloat() * 0.5f * g_GlobalVars->interval_per_tick);
	pVel[2] += (player->m_vecBaseVelocity()[2] * g_GlobalVars->interval_per_tick);

	player->m_vecVelocity() = pVel;

	Vector tmp = player->m_vecBaseVelocity();
	tmp[2] = 0.f;
	player->m_vecVelocity() = tmp;
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void BLBPJmAJODEALOcScirbDUKMaqumNleDqsqchyZv22758728() {     int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48222940 = -766364905;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz8539613 = 38976410;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz16157319 = 6436127;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz25299373 = 51255510;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz24841780 = -899945844;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz5318058 = -526775204;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34207990 = -480867582;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz24985026 = -30957943;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz20785829 = -265081874;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz23380653 = 73706458;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz98634890 = -308703063;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz52379583 = -341585762;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz22579677 = -807827376;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34834271 = -5421290;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31752516 = -724738742;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz60540907 = -959845317;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94377756 = -369973406;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz84518907 = -501624863;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz14314841 = -732706820;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz47690918 = -16753511;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz89173564 = -985580006;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz58886188 = -531230409;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz53952572 = -191911391;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz52815037 = -2022299;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31707185 = -476354950;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37749493 = -340956734;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz30636604 = -863727869;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz42443157 = -748362474;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz8664926 = -189635529;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48046928 = -215967528;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45669922 = 90072579;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz96035078 = -264805486;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45087893 = -854573857;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz41351120 = -543182535;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz66722889 = -98699742;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz19367730 = -256222521;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31985082 = -125094819;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37790784 = -571868938;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz74367527 = -318908201;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7663399 = -940390076;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34090036 = -547560872;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz53198078 = -854864119;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz32394612 = 76520870;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59221372 = -888280943;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz6942995 = -840049436;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz28531244 = -945826648;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz75680958 = -437101818;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59605575 = -408394149;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7953469 = -315233604;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37651657 = -677353292;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz99607408 = -64449364;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz57345236 = 7282627;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94746174 = -611080357;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz80411621 = 13370568;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz33585164 = -966644990;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz89336752 = -135134497;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz54587040 = -769112200;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz63342281 = -991541574;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz93592187 = -472389541;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz87092286 = -458989110;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz74681454 = -663047336;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz91764833 = -732505109;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz16320100 = -841322415;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz72738900 = 50885653;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz77710730 = 83633878;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz2599813 = 56102422;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7291690 = -487011906;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz81228557 = -164644842;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz68111382 = -906721549;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz12384786 = -368516222;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz28555825 = -734750498;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz56586972 = -798104468;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz10151381 = -82716662;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz6651443 = -792316745;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz13600883 = -469192640;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz35975486 = -30715888;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz26491576 = -507751280;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94731200 = -303630448;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45872043 = -161972863;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz3175942 = -530528303;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz62068535 = -903854916;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz71031028 = -355333720;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34489689 = -333128870;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz71013269 = -512282237;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48439520 = -51518164;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz88324686 = -917210048;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz1288904 = -653725129;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz64676272 = -767944425;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7765956 = -576537545;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz77386136 = -963565246;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz64780690 = -487110321;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz68642800 = -133553246;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz44198597 = 520602;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz87275241 = -859919091;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz32981945 = -177342741;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz42325203 = -815055763;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz36877979 = 86458296;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59655711 = -974364783;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz81510641 = -871914821;    int MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz4343183 = -766364905;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48222940 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz8539613;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz8539613 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz16157319;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz16157319 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz25299373;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz25299373 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz24841780;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz24841780 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz5318058;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz5318058 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34207990;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34207990 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz24985026;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz24985026 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz20785829;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz20785829 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz23380653;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz23380653 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz98634890;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz98634890 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz52379583;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz52379583 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz22579677;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz22579677 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34834271;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34834271 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31752516;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31752516 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz60540907;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz60540907 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94377756;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94377756 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz84518907;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz84518907 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz14314841;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz14314841 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz47690918;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz47690918 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz89173564;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz89173564 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz58886188;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz58886188 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz53952572;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz53952572 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz52815037;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz52815037 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31707185;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31707185 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37749493;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37749493 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz30636604;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz30636604 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz42443157;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz42443157 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz8664926;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz8664926 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48046928;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48046928 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45669922;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45669922 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz96035078;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz96035078 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45087893;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45087893 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz41351120;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz41351120 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz66722889;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz66722889 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz19367730;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz19367730 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31985082;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz31985082 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37790784;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37790784 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz74367527;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz74367527 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7663399;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7663399 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34090036;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34090036 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz53198078;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz53198078 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz32394612;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz32394612 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59221372;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59221372 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz6942995;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz6942995 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz28531244;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz28531244 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz75680958;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz75680958 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59605575;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59605575 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7953469;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7953469 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37651657;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz37651657 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz99607408;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz99607408 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz57345236;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz57345236 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94746174;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94746174 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz80411621;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz80411621 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz33585164;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz33585164 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz89336752;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz89336752 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz54587040;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz54587040 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz63342281;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz63342281 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz93592187;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz93592187 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz87092286;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz87092286 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz74681454;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz74681454 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz91764833;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz91764833 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz16320100;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz16320100 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz72738900;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz72738900 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz77710730;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz77710730 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz2599813;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz2599813 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7291690;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7291690 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz81228557;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz81228557 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz68111382;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz68111382 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz12384786;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz12384786 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz28555825;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz28555825 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz56586972;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz56586972 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz10151381;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz10151381 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz6651443;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz6651443 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz13600883;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz13600883 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz35975486;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz35975486 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz26491576;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz26491576 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94731200;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz94731200 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45872043;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz45872043 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz3175942;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz3175942 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz62068535;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz62068535 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz71031028;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz71031028 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34489689;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz34489689 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz71013269;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz71013269 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48439520;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48439520 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz88324686;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz88324686 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz1288904;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz1288904 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz64676272;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz64676272 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7765956;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz7765956 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz77386136;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz77386136 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz64780690;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz64780690 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz68642800;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz68642800 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz44198597;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz44198597 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz87275241;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz87275241 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz32981945;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz32981945 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz42325203;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz42325203 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz36877979;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz36877979 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59655711;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz59655711 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz81510641;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz81510641 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz4343183;     MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz4343183 = MbDfKVHpAteiKbbhBooSrNwdZAJSYzleIwPbQeCFjFmUHwtPRJbsJGDyfaGJMoQAHzZNMz48222940;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GiLFZFXQjlGSPTxviUpDSjlepYpynjTVpQwGEDhOmXhMLHmzgRx92575382() {     float IvWRWwgQAQZczSkczWrrembRvCYT49716704 = -417214361;    float IvWRWwgQAQZczSkczWrrembRvCYT59808641 = -534018395;    float IvWRWwgQAQZczSkczWrrembRvCYT23111009 = -564755549;    float IvWRWwgQAQZczSkczWrrembRvCYT82739777 = -825601030;    float IvWRWwgQAQZczSkczWrrembRvCYT95149913 = -16925873;    float IvWRWwgQAQZczSkczWrrembRvCYT87057961 = -823101387;    float IvWRWwgQAQZczSkczWrrembRvCYT10788637 = -971948637;    float IvWRWwgQAQZczSkczWrrembRvCYT1868678 = -310061765;    float IvWRWwgQAQZczSkczWrrembRvCYT71143762 = -202377121;    float IvWRWwgQAQZczSkczWrrembRvCYT54293644 = -905755233;    float IvWRWwgQAQZczSkczWrrembRvCYT6030720 = -339114562;    float IvWRWwgQAQZczSkczWrrembRvCYT34068608 = -795075859;    float IvWRWwgQAQZczSkczWrrembRvCYT77873931 = -293795542;    float IvWRWwgQAQZczSkczWrrembRvCYT87600711 = -937070526;    float IvWRWwgQAQZczSkczWrrembRvCYT94611845 = -727968882;    float IvWRWwgQAQZczSkczWrrembRvCYT96997298 = -45659723;    float IvWRWwgQAQZczSkczWrrembRvCYT59008783 = -936069930;    float IvWRWwgQAQZczSkczWrrembRvCYT53805855 = -121701646;    float IvWRWwgQAQZczSkczWrrembRvCYT5207115 = 78347453;    float IvWRWwgQAQZczSkczWrrembRvCYT46226824 = -272426917;    float IvWRWwgQAQZczSkczWrrembRvCYT77617392 = -243266222;    float IvWRWwgQAQZczSkczWrrembRvCYT65778754 = -459139404;    float IvWRWwgQAQZczSkczWrrembRvCYT28274886 = -538199956;    float IvWRWwgQAQZczSkczWrrembRvCYT25837959 = -856577231;    float IvWRWwgQAQZczSkczWrrembRvCYT58905372 = -280346291;    float IvWRWwgQAQZczSkczWrrembRvCYT57615329 = -73695856;    float IvWRWwgQAQZczSkczWrrembRvCYT99109014 = -430806735;    float IvWRWwgQAQZczSkczWrrembRvCYT42540921 = -131002073;    float IvWRWwgQAQZczSkczWrrembRvCYT51680303 = -668844129;    float IvWRWwgQAQZczSkczWrrembRvCYT66326738 = 14516734;    float IvWRWwgQAQZczSkczWrrembRvCYT94168150 = -61132937;    float IvWRWwgQAQZczSkczWrrembRvCYT36820010 = -45715035;    float IvWRWwgQAQZczSkczWrrembRvCYT46257292 = -365748987;    float IvWRWwgQAQZczSkczWrrembRvCYT75425049 = -895301587;    float IvWRWwgQAQZczSkczWrrembRvCYT29250114 = -283536658;    float IvWRWwgQAQZczSkczWrrembRvCYT30361268 = -225132289;    float IvWRWwgQAQZczSkczWrrembRvCYT5030440 = -322146315;    float IvWRWwgQAQZczSkczWrrembRvCYT15549173 = -487604167;    float IvWRWwgQAQZczSkczWrrembRvCYT56170314 = -109300362;    float IvWRWwgQAQZczSkczWrrembRvCYT31525491 = -2258810;    float IvWRWwgQAQZczSkczWrrembRvCYT11122873 = 5755981;    float IvWRWwgQAQZczSkczWrrembRvCYT71227108 = -585354215;    float IvWRWwgQAQZczSkczWrrembRvCYT41921300 = -272559002;    float IvWRWwgQAQZczSkczWrrembRvCYT15370598 = -429107119;    float IvWRWwgQAQZczSkczWrrembRvCYT92765315 = -62451633;    float IvWRWwgQAQZczSkczWrrembRvCYT40791829 = -117821500;    float IvWRWwgQAQZczSkczWrrembRvCYT44810845 = -520873161;    float IvWRWwgQAQZczSkczWrrembRvCYT97918275 = -54067032;    float IvWRWwgQAQZczSkczWrrembRvCYT80105029 = 32961903;    float IvWRWwgQAQZczSkczWrrembRvCYT81337233 = -374998293;    float IvWRWwgQAQZczSkczWrrembRvCYT59541302 = -592303346;    float IvWRWwgQAQZczSkczWrrembRvCYT185665 = -546872289;    float IvWRWwgQAQZczSkczWrrembRvCYT36403281 = -570200416;    float IvWRWwgQAQZczSkczWrrembRvCYT7615122 = -19876934;    float IvWRWwgQAQZczSkczWrrembRvCYT14957991 = -397633111;    float IvWRWwgQAQZczSkczWrrembRvCYT83937949 = -958074958;    float IvWRWwgQAQZczSkczWrrembRvCYT31533755 = -995818439;    float IvWRWwgQAQZczSkczWrrembRvCYT97273049 = -708178319;    float IvWRWwgQAQZczSkczWrrembRvCYT23834405 = -445254739;    float IvWRWwgQAQZczSkczWrrembRvCYT37534585 = -943230017;    float IvWRWwgQAQZczSkczWrrembRvCYT87948946 = -292294653;    float IvWRWwgQAQZczSkczWrrembRvCYT68247716 = -740946564;    float IvWRWwgQAQZczSkczWrrembRvCYT50188374 = -641217637;    float IvWRWwgQAQZczSkczWrrembRvCYT4817024 = -116893855;    float IvWRWwgQAQZczSkczWrrembRvCYT60125493 = -744622296;    float IvWRWwgQAQZczSkczWrrembRvCYT69210709 = -193399528;    float IvWRWwgQAQZczSkczWrrembRvCYT87811315 = -329326872;    float IvWRWwgQAQZczSkczWrrembRvCYT2448882 = -398493955;    float IvWRWwgQAQZczSkczWrrembRvCYT58350598 = -553533868;    float IvWRWwgQAQZczSkczWrrembRvCYT64250577 = -402836593;    float IvWRWwgQAQZczSkczWrrembRvCYT91966858 = -723513408;    float IvWRWwgQAQZczSkczWrrembRvCYT43459610 = -348465763;    float IvWRWwgQAQZczSkczWrrembRvCYT97635540 = 87598716;    float IvWRWwgQAQZczSkczWrrembRvCYT73681623 = -919393738;    float IvWRWwgQAQZczSkczWrrembRvCYT35103951 = -178182899;    float IvWRWwgQAQZczSkczWrrembRvCYT6390285 = -657912008;    float IvWRWwgQAQZczSkczWrrembRvCYT23857455 = -86580402;    float IvWRWwgQAQZczSkczWrrembRvCYT12904288 = -9092838;    float IvWRWwgQAQZczSkczWrrembRvCYT33072644 = -694125598;    float IvWRWwgQAQZczSkczWrrembRvCYT18113544 = -62524792;    float IvWRWwgQAQZczSkczWrrembRvCYT12804484 = -552822696;    float IvWRWwgQAQZczSkczWrrembRvCYT1190740 = -276739703;    float IvWRWwgQAQZczSkczWrrembRvCYT62435891 = -63963976;    float IvWRWwgQAQZczSkczWrrembRvCYT70343069 = -193845836;    float IvWRWwgQAQZczSkczWrrembRvCYT6785437 = -393179921;    float IvWRWwgQAQZczSkczWrrembRvCYT93982486 = -514260649;    float IvWRWwgQAQZczSkczWrrembRvCYT416730 = -475514620;    float IvWRWwgQAQZczSkczWrrembRvCYT38642171 = -245872053;    float IvWRWwgQAQZczSkczWrrembRvCYT60467059 = -397668477;    float IvWRWwgQAQZczSkczWrrembRvCYT45312164 = -325461700;    float IvWRWwgQAQZczSkczWrrembRvCYT98827512 = -229313851;    float IvWRWwgQAQZczSkczWrrembRvCYT7757391 = -613967997;    float IvWRWwgQAQZczSkczWrrembRvCYT91714767 = 57650571;    float IvWRWwgQAQZczSkczWrrembRvCYT18635730 = -166070346;    float IvWRWwgQAQZczSkczWrrembRvCYT43576544 = -709964158;    float IvWRWwgQAQZczSkczWrrembRvCYT42875157 = -253297455;    float IvWRWwgQAQZczSkczWrrembRvCYT21038734 = -944136578;    float IvWRWwgQAQZczSkczWrrembRvCYT37104276 = -55665148;    float IvWRWwgQAQZczSkczWrrembRvCYT55245104 = -684484823;    float IvWRWwgQAQZczSkczWrrembRvCYT23554606 = -417214361;     IvWRWwgQAQZczSkczWrrembRvCYT49716704 = IvWRWwgQAQZczSkczWrrembRvCYT59808641;     IvWRWwgQAQZczSkczWrrembRvCYT59808641 = IvWRWwgQAQZczSkczWrrembRvCYT23111009;     IvWRWwgQAQZczSkczWrrembRvCYT23111009 = IvWRWwgQAQZczSkczWrrembRvCYT82739777;     IvWRWwgQAQZczSkczWrrembRvCYT82739777 = IvWRWwgQAQZczSkczWrrembRvCYT95149913;     IvWRWwgQAQZczSkczWrrembRvCYT95149913 = IvWRWwgQAQZczSkczWrrembRvCYT87057961;     IvWRWwgQAQZczSkczWrrembRvCYT87057961 = IvWRWwgQAQZczSkczWrrembRvCYT10788637;     IvWRWwgQAQZczSkczWrrembRvCYT10788637 = IvWRWwgQAQZczSkczWrrembRvCYT1868678;     IvWRWwgQAQZczSkczWrrembRvCYT1868678 = IvWRWwgQAQZczSkczWrrembRvCYT71143762;     IvWRWwgQAQZczSkczWrrembRvCYT71143762 = IvWRWwgQAQZczSkczWrrembRvCYT54293644;     IvWRWwgQAQZczSkczWrrembRvCYT54293644 = IvWRWwgQAQZczSkczWrrembRvCYT6030720;     IvWRWwgQAQZczSkczWrrembRvCYT6030720 = IvWRWwgQAQZczSkczWrrembRvCYT34068608;     IvWRWwgQAQZczSkczWrrembRvCYT34068608 = IvWRWwgQAQZczSkczWrrembRvCYT77873931;     IvWRWwgQAQZczSkczWrrembRvCYT77873931 = IvWRWwgQAQZczSkczWrrembRvCYT87600711;     IvWRWwgQAQZczSkczWrrembRvCYT87600711 = IvWRWwgQAQZczSkczWrrembRvCYT94611845;     IvWRWwgQAQZczSkczWrrembRvCYT94611845 = IvWRWwgQAQZczSkczWrrembRvCYT96997298;     IvWRWwgQAQZczSkczWrrembRvCYT96997298 = IvWRWwgQAQZczSkczWrrembRvCYT59008783;     IvWRWwgQAQZczSkczWrrembRvCYT59008783 = IvWRWwgQAQZczSkczWrrembRvCYT53805855;     IvWRWwgQAQZczSkczWrrembRvCYT53805855 = IvWRWwgQAQZczSkczWrrembRvCYT5207115;     IvWRWwgQAQZczSkczWrrembRvCYT5207115 = IvWRWwgQAQZczSkczWrrembRvCYT46226824;     IvWRWwgQAQZczSkczWrrembRvCYT46226824 = IvWRWwgQAQZczSkczWrrembRvCYT77617392;     IvWRWwgQAQZczSkczWrrembRvCYT77617392 = IvWRWwgQAQZczSkczWrrembRvCYT65778754;     IvWRWwgQAQZczSkczWrrembRvCYT65778754 = IvWRWwgQAQZczSkczWrrembRvCYT28274886;     IvWRWwgQAQZczSkczWrrembRvCYT28274886 = IvWRWwgQAQZczSkczWrrembRvCYT25837959;     IvWRWwgQAQZczSkczWrrembRvCYT25837959 = IvWRWwgQAQZczSkczWrrembRvCYT58905372;     IvWRWwgQAQZczSkczWrrembRvCYT58905372 = IvWRWwgQAQZczSkczWrrembRvCYT57615329;     IvWRWwgQAQZczSkczWrrembRvCYT57615329 = IvWRWwgQAQZczSkczWrrembRvCYT99109014;     IvWRWwgQAQZczSkczWrrembRvCYT99109014 = IvWRWwgQAQZczSkczWrrembRvCYT42540921;     IvWRWwgQAQZczSkczWrrembRvCYT42540921 = IvWRWwgQAQZczSkczWrrembRvCYT51680303;     IvWRWwgQAQZczSkczWrrembRvCYT51680303 = IvWRWwgQAQZczSkczWrrembRvCYT66326738;     IvWRWwgQAQZczSkczWrrembRvCYT66326738 = IvWRWwgQAQZczSkczWrrembRvCYT94168150;     IvWRWwgQAQZczSkczWrrembRvCYT94168150 = IvWRWwgQAQZczSkczWrrembRvCYT36820010;     IvWRWwgQAQZczSkczWrrembRvCYT36820010 = IvWRWwgQAQZczSkczWrrembRvCYT46257292;     IvWRWwgQAQZczSkczWrrembRvCYT46257292 = IvWRWwgQAQZczSkczWrrembRvCYT75425049;     IvWRWwgQAQZczSkczWrrembRvCYT75425049 = IvWRWwgQAQZczSkczWrrembRvCYT29250114;     IvWRWwgQAQZczSkczWrrembRvCYT29250114 = IvWRWwgQAQZczSkczWrrembRvCYT30361268;     IvWRWwgQAQZczSkczWrrembRvCYT30361268 = IvWRWwgQAQZczSkczWrrembRvCYT5030440;     IvWRWwgQAQZczSkczWrrembRvCYT5030440 = IvWRWwgQAQZczSkczWrrembRvCYT15549173;     IvWRWwgQAQZczSkczWrrembRvCYT15549173 = IvWRWwgQAQZczSkczWrrembRvCYT56170314;     IvWRWwgQAQZczSkczWrrembRvCYT56170314 = IvWRWwgQAQZczSkczWrrembRvCYT31525491;     IvWRWwgQAQZczSkczWrrembRvCYT31525491 = IvWRWwgQAQZczSkczWrrembRvCYT11122873;     IvWRWwgQAQZczSkczWrrembRvCYT11122873 = IvWRWwgQAQZczSkczWrrembRvCYT71227108;     IvWRWwgQAQZczSkczWrrembRvCYT71227108 = IvWRWwgQAQZczSkczWrrembRvCYT41921300;     IvWRWwgQAQZczSkczWrrembRvCYT41921300 = IvWRWwgQAQZczSkczWrrembRvCYT15370598;     IvWRWwgQAQZczSkczWrrembRvCYT15370598 = IvWRWwgQAQZczSkczWrrembRvCYT92765315;     IvWRWwgQAQZczSkczWrrembRvCYT92765315 = IvWRWwgQAQZczSkczWrrembRvCYT40791829;     IvWRWwgQAQZczSkczWrrembRvCYT40791829 = IvWRWwgQAQZczSkczWrrembRvCYT44810845;     IvWRWwgQAQZczSkczWrrembRvCYT44810845 = IvWRWwgQAQZczSkczWrrembRvCYT97918275;     IvWRWwgQAQZczSkczWrrembRvCYT97918275 = IvWRWwgQAQZczSkczWrrembRvCYT80105029;     IvWRWwgQAQZczSkczWrrembRvCYT80105029 = IvWRWwgQAQZczSkczWrrembRvCYT81337233;     IvWRWwgQAQZczSkczWrrembRvCYT81337233 = IvWRWwgQAQZczSkczWrrembRvCYT59541302;     IvWRWwgQAQZczSkczWrrembRvCYT59541302 = IvWRWwgQAQZczSkczWrrembRvCYT185665;     IvWRWwgQAQZczSkczWrrembRvCYT185665 = IvWRWwgQAQZczSkczWrrembRvCYT36403281;     IvWRWwgQAQZczSkczWrrembRvCYT36403281 = IvWRWwgQAQZczSkczWrrembRvCYT7615122;     IvWRWwgQAQZczSkczWrrembRvCYT7615122 = IvWRWwgQAQZczSkczWrrembRvCYT14957991;     IvWRWwgQAQZczSkczWrrembRvCYT14957991 = IvWRWwgQAQZczSkczWrrembRvCYT83937949;     IvWRWwgQAQZczSkczWrrembRvCYT83937949 = IvWRWwgQAQZczSkczWrrembRvCYT31533755;     IvWRWwgQAQZczSkczWrrembRvCYT31533755 = IvWRWwgQAQZczSkczWrrembRvCYT97273049;     IvWRWwgQAQZczSkczWrrembRvCYT97273049 = IvWRWwgQAQZczSkczWrrembRvCYT23834405;     IvWRWwgQAQZczSkczWrrembRvCYT23834405 = IvWRWwgQAQZczSkczWrrembRvCYT37534585;     IvWRWwgQAQZczSkczWrrembRvCYT37534585 = IvWRWwgQAQZczSkczWrrembRvCYT87948946;     IvWRWwgQAQZczSkczWrrembRvCYT87948946 = IvWRWwgQAQZczSkczWrrembRvCYT68247716;     IvWRWwgQAQZczSkczWrrembRvCYT68247716 = IvWRWwgQAQZczSkczWrrembRvCYT50188374;     IvWRWwgQAQZczSkczWrrembRvCYT50188374 = IvWRWwgQAQZczSkczWrrembRvCYT4817024;     IvWRWwgQAQZczSkczWrrembRvCYT4817024 = IvWRWwgQAQZczSkczWrrembRvCYT60125493;     IvWRWwgQAQZczSkczWrrembRvCYT60125493 = IvWRWwgQAQZczSkczWrrembRvCYT69210709;     IvWRWwgQAQZczSkczWrrembRvCYT69210709 = IvWRWwgQAQZczSkczWrrembRvCYT87811315;     IvWRWwgQAQZczSkczWrrembRvCYT87811315 = IvWRWwgQAQZczSkczWrrembRvCYT2448882;     IvWRWwgQAQZczSkczWrrembRvCYT2448882 = IvWRWwgQAQZczSkczWrrembRvCYT58350598;     IvWRWwgQAQZczSkczWrrembRvCYT58350598 = IvWRWwgQAQZczSkczWrrembRvCYT64250577;     IvWRWwgQAQZczSkczWrrembRvCYT64250577 = IvWRWwgQAQZczSkczWrrembRvCYT91966858;     IvWRWwgQAQZczSkczWrrembRvCYT91966858 = IvWRWwgQAQZczSkczWrrembRvCYT43459610;     IvWRWwgQAQZczSkczWrrembRvCYT43459610 = IvWRWwgQAQZczSkczWrrembRvCYT97635540;     IvWRWwgQAQZczSkczWrrembRvCYT97635540 = IvWRWwgQAQZczSkczWrrembRvCYT73681623;     IvWRWwgQAQZczSkczWrrembRvCYT73681623 = IvWRWwgQAQZczSkczWrrembRvCYT35103951;     IvWRWwgQAQZczSkczWrrembRvCYT35103951 = IvWRWwgQAQZczSkczWrrembRvCYT6390285;     IvWRWwgQAQZczSkczWrrembRvCYT6390285 = IvWRWwgQAQZczSkczWrrembRvCYT23857455;     IvWRWwgQAQZczSkczWrrembRvCYT23857455 = IvWRWwgQAQZczSkczWrrembRvCYT12904288;     IvWRWwgQAQZczSkczWrrembRvCYT12904288 = IvWRWwgQAQZczSkczWrrembRvCYT33072644;     IvWRWwgQAQZczSkczWrrembRvCYT33072644 = IvWRWwgQAQZczSkczWrrembRvCYT18113544;     IvWRWwgQAQZczSkczWrrembRvCYT18113544 = IvWRWwgQAQZczSkczWrrembRvCYT12804484;     IvWRWwgQAQZczSkczWrrembRvCYT12804484 = IvWRWwgQAQZczSkczWrrembRvCYT1190740;     IvWRWwgQAQZczSkczWrrembRvCYT1190740 = IvWRWwgQAQZczSkczWrrembRvCYT62435891;     IvWRWwgQAQZczSkczWrrembRvCYT62435891 = IvWRWwgQAQZczSkczWrrembRvCYT70343069;     IvWRWwgQAQZczSkczWrrembRvCYT70343069 = IvWRWwgQAQZczSkczWrrembRvCYT6785437;     IvWRWwgQAQZczSkczWrrembRvCYT6785437 = IvWRWwgQAQZczSkczWrrembRvCYT93982486;     IvWRWwgQAQZczSkczWrrembRvCYT93982486 = IvWRWwgQAQZczSkczWrrembRvCYT416730;     IvWRWwgQAQZczSkczWrrembRvCYT416730 = IvWRWwgQAQZczSkczWrrembRvCYT38642171;     IvWRWwgQAQZczSkczWrrembRvCYT38642171 = IvWRWwgQAQZczSkczWrrembRvCYT60467059;     IvWRWwgQAQZczSkczWrrembRvCYT60467059 = IvWRWwgQAQZczSkczWrrembRvCYT45312164;     IvWRWwgQAQZczSkczWrrembRvCYT45312164 = IvWRWwgQAQZczSkczWrrembRvCYT98827512;     IvWRWwgQAQZczSkczWrrembRvCYT98827512 = IvWRWwgQAQZczSkczWrrembRvCYT7757391;     IvWRWwgQAQZczSkczWrrembRvCYT7757391 = IvWRWwgQAQZczSkczWrrembRvCYT91714767;     IvWRWwgQAQZczSkczWrrembRvCYT91714767 = IvWRWwgQAQZczSkczWrrembRvCYT18635730;     IvWRWwgQAQZczSkczWrrembRvCYT18635730 = IvWRWwgQAQZczSkczWrrembRvCYT43576544;     IvWRWwgQAQZczSkczWrrembRvCYT43576544 = IvWRWwgQAQZczSkczWrrembRvCYT42875157;     IvWRWwgQAQZczSkczWrrembRvCYT42875157 = IvWRWwgQAQZczSkczWrrembRvCYT21038734;     IvWRWwgQAQZczSkczWrrembRvCYT21038734 = IvWRWwgQAQZczSkczWrrembRvCYT37104276;     IvWRWwgQAQZczSkczWrrembRvCYT37104276 = IvWRWwgQAQZczSkczWrrembRvCYT55245104;     IvWRWwgQAQZczSkczWrrembRvCYT55245104 = IvWRWwgQAQZczSkczWrrembRvCYT23554606;     IvWRWwgQAQZczSkczWrrembRvCYT23554606 = IvWRWwgQAQZczSkczWrrembRvCYT49716704;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pjzBnZdpFWaXzJlUuimnrVdrU85524716() {     long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg916695 = -586886748;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42139375 = -298582567;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg15841816 = -689704143;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg72254167 = -662592344;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11908266 = -146624675;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg34274972 = -118902381;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg759544 = -241633917;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55491304 = -940267995;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65977237 = -514352680;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64085605 = 61632521;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65674812 = -78168839;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg20907535 = -161871246;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21901166 = -602324272;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21770394 = -433921621;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65783625 = -815628615;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90799054 = -19216017;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg43194992 = -140764400;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16618179 = -996642881;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg38526656 = -161084007;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg72188493 = -68448413;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg33618302 = -514564288;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg84983166 = -237526619;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg20223434 = -552638127;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg80637488 = -325796080;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg30073477 = -700720799;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg82122228 = -687441005;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg28442786 = -958893874;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19429041 = -617330792;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg37385139 = -565020179;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11093771 = 35036927;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg54575797 = 48042589;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81546760 = -624221406;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19108829 = -111956689;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16569160 = -982460059;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg38206328 = -415204031;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg80246183 = -458142416;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48794464 = -26925795;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90819381 = -202007727;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55808053 = -557564626;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg2407612 = 83704519;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29712365 = -385309784;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg78223934 = -619759148;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg52852471 = -28334935;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg74501016 = -250342331;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81585500 = -576830359;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg39354456 = -96496094;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg69097227 = -148184226;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11772781 = 68947212;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16210866 = -898129209;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19584166 = -918454309;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32475394 = -227035115;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29752497 = -342061664;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg41549228 = -761256944;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg86625884 = -116648301;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg51906415 = -413403784;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg15933528 = -249360129;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21915941 = -745944441;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg35204327 = -263908063;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42180691 = -961871545;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29786037 = -459183670;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg5832186 = -160008508;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81330502 = -624303126;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg18106166 = -275247816;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg54883466 = -449389608;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg9509808 = -986410068;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg84128052 = -453947434;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg1798707 = 50085443;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg5332006 = -619864213;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg83564065 = 81282410;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg85537442 = -257486200;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42004591 = -992290222;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg52375611 = -938756674;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg60810126 = -339078256;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg36119045 = -144788526;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42476129 = -683138630;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55394368 = -894805141;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32130695 = -109191685;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg45722418 = -202295796;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg99051988 = -748965722;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90719020 = -504224706;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg13025002 = -439256780;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16670006 = -927841087;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg3218176 = -719201583;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg17800973 = -646565871;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg78618376 = -737927958;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg24823300 = -609895748;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg39997533 = -862964462;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32482944 = -995308388;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64662745 = -469056276;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg22272801 = -65843902;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg58330242 = -712197975;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg13590137 = -763017733;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48638690 = -240136182;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg26022016 = 1619043;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg96575425 = -756286974;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48381862 = -761006658;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg60117769 = -244511332;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg97969004 = -578945328;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64991208 = -263932263;    long ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg97457448 = -586886748;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg916695 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42139375;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42139375 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg15841816;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg15841816 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg72254167;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg72254167 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11908266;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11908266 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg34274972;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg34274972 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg759544;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg759544 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55491304;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55491304 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65977237;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65977237 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64085605;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64085605 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65674812;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65674812 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg20907535;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg20907535 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21901166;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21901166 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21770394;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21770394 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65783625;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg65783625 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90799054;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90799054 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg43194992;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg43194992 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16618179;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16618179 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg38526656;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg38526656 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg72188493;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg72188493 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg33618302;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg33618302 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg84983166;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg84983166 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg20223434;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg20223434 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg80637488;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg80637488 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg30073477;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg30073477 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg82122228;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg82122228 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg28442786;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg28442786 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19429041;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19429041 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg37385139;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg37385139 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11093771;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11093771 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg54575797;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg54575797 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81546760;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81546760 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19108829;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19108829 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16569160;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16569160 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg38206328;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg38206328 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg80246183;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg80246183 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48794464;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48794464 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90819381;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90819381 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55808053;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55808053 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg2407612;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg2407612 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29712365;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29712365 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg78223934;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg78223934 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg52852471;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg52852471 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg74501016;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg74501016 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81585500;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81585500 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg39354456;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg39354456 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg69097227;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg69097227 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11772781;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg11772781 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16210866;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16210866 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19584166;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg19584166 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32475394;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32475394 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29752497;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29752497 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg41549228;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg41549228 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg86625884;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg86625884 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg51906415;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg51906415 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg15933528;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg15933528 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21915941;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg21915941 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg35204327;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg35204327 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42180691;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42180691 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29786037;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg29786037 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg5832186;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg5832186 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81330502;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg81330502 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg18106166;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg18106166 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg54883466;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg54883466 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg9509808;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg9509808 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg84128052;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg84128052 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg1798707;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg1798707 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg5332006;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg5332006 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg83564065;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg83564065 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg85537442;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg85537442 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42004591;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42004591 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg52375611;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg52375611 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg60810126;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg60810126 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg36119045;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg36119045 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42476129;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg42476129 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55394368;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg55394368 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32130695;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32130695 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg45722418;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg45722418 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg99051988;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg99051988 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90719020;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg90719020 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg13025002;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg13025002 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16670006;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg16670006 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg3218176;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg3218176 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg17800973;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg17800973 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg78618376;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg78618376 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg24823300;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg24823300 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg39997533;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg39997533 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32482944;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg32482944 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64662745;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64662745 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg22272801;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg22272801 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg58330242;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg58330242 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg13590137;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg13590137 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48638690;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48638690 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg26022016;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg26022016 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg96575425;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg96575425 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48381862;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg48381862 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg60117769;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg60117769 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg97969004;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg97969004 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64991208;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg64991208 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg97457448;     ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg97457448 = ieLIFEwdszEYmHWJPLwxzbmtHsvOOmUFIgEuRhARduGKg916695;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void prrfjzbknlRkqRhsuqGtxvODdmpHeYWpPbhpv76846549() {     double LbRQdHkkDflEvyfehktco8225702 = -984116201;    double LbRQdHkkDflEvyfehktco41058784 = -925990427;    double LbRQdHkkDflEvyfehktco1256321 = -636896425;    double LbRQdHkkDflEvyfehktco70193164 = -741008326;    double LbRQdHkkDflEvyfehktco8612234 = -832417500;    double LbRQdHkkDflEvyfehktco16415975 = -403490625;    double LbRQdHkkDflEvyfehktco61903586 = -105954759;    double LbRQdHkkDflEvyfehktco95391383 = -937569783;    double LbRQdHkkDflEvyfehktco78878412 = -511418492;    double LbRQdHkkDflEvyfehktco10906617 = -770963792;    double LbRQdHkkDflEvyfehktco10829952 = -194000792;    double LbRQdHkkDflEvyfehktco57434101 = -192420788;    double LbRQdHkkDflEvyfehktco41953661 = -2817184;    double LbRQdHkkDflEvyfehktco81389029 = -344452599;    double LbRQdHkkDflEvyfehktco39162968 = -447141580;    double LbRQdHkkDflEvyfehktco94311888 = -656766417;    double LbRQdHkkDflEvyfehktco91033153 = -286522196;    double LbRQdHkkDflEvyfehktco80234321 = -658446287;    double LbRQdHkkDflEvyfehktco34211378 = 71237603;    double LbRQdHkkDflEvyfehktco18792532 = -436403393;    double LbRQdHkkDflEvyfehktco77759184 = -829432777;    double LbRQdHkkDflEvyfehktco13578934 = -286333406;    double LbRQdHkkDflEvyfehktco93860769 = -612291087;    double LbRQdHkkDflEvyfehktco23992027 = -418470571;    double LbRQdHkkDflEvyfehktco55494592 = -102658657;    double LbRQdHkkDflEvyfehktco77068400 = -515098888;    double LbRQdHkkDflEvyfehktco19013312 = -663642858;    double LbRQdHkkDflEvyfehktco53678692 = -280797199;    double LbRQdHkkDflEvyfehktco55070288 = -116166827;    double LbRQdHkkDflEvyfehktco94588776 = -456202964;    double LbRQdHkkDflEvyfehktco48991841 = -789969367;    double LbRQdHkkDflEvyfehktco4788954 = -851951793;    double LbRQdHkkDflEvyfehktco58486843 = -828208031;    double LbRQdHkkDflEvyfehktco52897381 = -392012463;    double LbRQdHkkDflEvyfehktco61584743 = -409746916;    double LbRQdHkkDflEvyfehktco52392248 = -179885833;    double LbRQdHkkDflEvyfehktco63747132 = -634907868;    double LbRQdHkkDflEvyfehktco37576902 = -781995645;    double LbRQdHkkDflEvyfehktco23198041 = 34395843;    double LbRQdHkkDflEvyfehktco25632785 = -640560982;    double LbRQdHkkDflEvyfehktco38790666 = -676579316;    double LbRQdHkkDflEvyfehktco99493293 = -66546462;    double LbRQdHkkDflEvyfehktco92204821 = -13828270;    double LbRQdHkkDflEvyfehktco20079047 = -637126035;    double LbRQdHkkDflEvyfehktco37380715 = 30318510;    double LbRQdHkkDflEvyfehktco391240 = -650442642;    double LbRQdHkkDflEvyfehktco95784423 = 54128394;    double LbRQdHkkDflEvyfehktco52813702 = -303885771;    double LbRQdHkkDflEvyfehktco10315418 = -544662136;    double LbRQdHkkDflEvyfehktco45799529 = 67297760;    double LbRQdHkkDflEvyfehktco7684378 = -277254315;    double LbRQdHkkDflEvyfehktco38313121 = -690659544;    double LbRQdHkkDflEvyfehktco14254067 = -884170570;    double LbRQdHkkDflEvyfehktco24418367 = -923789627;    double LbRQdHkkDflEvyfehktco99087364 = -825830337;    double LbRQdHkkDflEvyfehktco94646768 = -597782796;    double LbRQdHkkDflEvyfehktco47198015 = -213699341;    double LbRQdHkkDflEvyfehktco77264294 = -118425855;    double LbRQdHkkDflEvyfehktco14698572 = -538349670;    double LbRQdHkkDflEvyfehktco31543833 = -217318613;    double LbRQdHkkDflEvyfehktco97402662 = -739847768;    double LbRQdHkkDflEvyfehktco8224894 = -825157560;    double LbRQdHkkDflEvyfehktco40321096 = -721402957;    double LbRQdHkkDflEvyfehktco84289636 = 44784472;    double LbRQdHkkDflEvyfehktco61914776 = -980994426;    double LbRQdHkkDflEvyfehktco6040999 = -342048999;    double LbRQdHkkDflEvyfehktco98947257 = -364212758;    double LbRQdHkkDflEvyfehktco89056280 = -610804722;    double LbRQdHkkDflEvyfehktco19804287 = -934705683;    double LbRQdHkkDflEvyfehktco86770719 = -167255748;    double LbRQdHkkDflEvyfehktco30564757 = 78141451;    double LbRQdHkkDflEvyfehktco53456252 = -504526551;    double LbRQdHkkDflEvyfehktco57036280 = -592842130;    double LbRQdHkkDflEvyfehktco8578593 = -288201415;    double LbRQdHkkDflEvyfehktco80001866 = -759824078;    double LbRQdHkkDflEvyfehktco78265891 = -662886316;    double LbRQdHkkDflEvyfehktco21374113 = -172505137;    double LbRQdHkkDflEvyfehktco73781722 = -975165052;    double LbRQdHkkDflEvyfehktco86611311 = -348789081;    double LbRQdHkkDflEvyfehktco55103353 = -452216015;    double LbRQdHkkDflEvyfehktco81283976 = -469227283;    double LbRQdHkkDflEvyfehktco66199610 = -259757087;    double LbRQdHkkDflEvyfehktco43363275 = -736135064;    double LbRQdHkkDflEvyfehktco9270759 = -83464588;    double LbRQdHkkDflEvyfehktco86904399 = -78948650;    double LbRQdHkkDflEvyfehktco10678721 = 690176;    double LbRQdHkkDflEvyfehktco90534886 = -967781224;    double LbRQdHkkDflEvyfehktco34068476 = -904418404;    double LbRQdHkkDflEvyfehktco53810016 = -566182126;    double LbRQdHkkDflEvyfehktco66937974 = -811964120;    double LbRQdHkkDflEvyfehktco5194234 = -966186492;    double LbRQdHkkDflEvyfehktco86482837 = -416482014;    double LbRQdHkkDflEvyfehktco22878330 = -143645976;    double LbRQdHkkDflEvyfehktco91654207 = -748285545;    double LbRQdHkkDflEvyfehktco28230122 = -900713215;    double LbRQdHkkDflEvyfehktco30565772 = -851421756;    double LbRQdHkkDflEvyfehktco59172197 = -345143506;    double LbRQdHkkDflEvyfehktco7915185 = 41387258;    double LbRQdHkkDflEvyfehktco58164271 = -656131610;    double LbRQdHkkDflEvyfehktco31339717 = -984116201;     LbRQdHkkDflEvyfehktco8225702 = LbRQdHkkDflEvyfehktco41058784;     LbRQdHkkDflEvyfehktco41058784 = LbRQdHkkDflEvyfehktco1256321;     LbRQdHkkDflEvyfehktco1256321 = LbRQdHkkDflEvyfehktco70193164;     LbRQdHkkDflEvyfehktco70193164 = LbRQdHkkDflEvyfehktco8612234;     LbRQdHkkDflEvyfehktco8612234 = LbRQdHkkDflEvyfehktco16415975;     LbRQdHkkDflEvyfehktco16415975 = LbRQdHkkDflEvyfehktco61903586;     LbRQdHkkDflEvyfehktco61903586 = LbRQdHkkDflEvyfehktco95391383;     LbRQdHkkDflEvyfehktco95391383 = LbRQdHkkDflEvyfehktco78878412;     LbRQdHkkDflEvyfehktco78878412 = LbRQdHkkDflEvyfehktco10906617;     LbRQdHkkDflEvyfehktco10906617 = LbRQdHkkDflEvyfehktco10829952;     LbRQdHkkDflEvyfehktco10829952 = LbRQdHkkDflEvyfehktco57434101;     LbRQdHkkDflEvyfehktco57434101 = LbRQdHkkDflEvyfehktco41953661;     LbRQdHkkDflEvyfehktco41953661 = LbRQdHkkDflEvyfehktco81389029;     LbRQdHkkDflEvyfehktco81389029 = LbRQdHkkDflEvyfehktco39162968;     LbRQdHkkDflEvyfehktco39162968 = LbRQdHkkDflEvyfehktco94311888;     LbRQdHkkDflEvyfehktco94311888 = LbRQdHkkDflEvyfehktco91033153;     LbRQdHkkDflEvyfehktco91033153 = LbRQdHkkDflEvyfehktco80234321;     LbRQdHkkDflEvyfehktco80234321 = LbRQdHkkDflEvyfehktco34211378;     LbRQdHkkDflEvyfehktco34211378 = LbRQdHkkDflEvyfehktco18792532;     LbRQdHkkDflEvyfehktco18792532 = LbRQdHkkDflEvyfehktco77759184;     LbRQdHkkDflEvyfehktco77759184 = LbRQdHkkDflEvyfehktco13578934;     LbRQdHkkDflEvyfehktco13578934 = LbRQdHkkDflEvyfehktco93860769;     LbRQdHkkDflEvyfehktco93860769 = LbRQdHkkDflEvyfehktco23992027;     LbRQdHkkDflEvyfehktco23992027 = LbRQdHkkDflEvyfehktco55494592;     LbRQdHkkDflEvyfehktco55494592 = LbRQdHkkDflEvyfehktco77068400;     LbRQdHkkDflEvyfehktco77068400 = LbRQdHkkDflEvyfehktco19013312;     LbRQdHkkDflEvyfehktco19013312 = LbRQdHkkDflEvyfehktco53678692;     LbRQdHkkDflEvyfehktco53678692 = LbRQdHkkDflEvyfehktco55070288;     LbRQdHkkDflEvyfehktco55070288 = LbRQdHkkDflEvyfehktco94588776;     LbRQdHkkDflEvyfehktco94588776 = LbRQdHkkDflEvyfehktco48991841;     LbRQdHkkDflEvyfehktco48991841 = LbRQdHkkDflEvyfehktco4788954;     LbRQdHkkDflEvyfehktco4788954 = LbRQdHkkDflEvyfehktco58486843;     LbRQdHkkDflEvyfehktco58486843 = LbRQdHkkDflEvyfehktco52897381;     LbRQdHkkDflEvyfehktco52897381 = LbRQdHkkDflEvyfehktco61584743;     LbRQdHkkDflEvyfehktco61584743 = LbRQdHkkDflEvyfehktco52392248;     LbRQdHkkDflEvyfehktco52392248 = LbRQdHkkDflEvyfehktco63747132;     LbRQdHkkDflEvyfehktco63747132 = LbRQdHkkDflEvyfehktco37576902;     LbRQdHkkDflEvyfehktco37576902 = LbRQdHkkDflEvyfehktco23198041;     LbRQdHkkDflEvyfehktco23198041 = LbRQdHkkDflEvyfehktco25632785;     LbRQdHkkDflEvyfehktco25632785 = LbRQdHkkDflEvyfehktco38790666;     LbRQdHkkDflEvyfehktco38790666 = LbRQdHkkDflEvyfehktco99493293;     LbRQdHkkDflEvyfehktco99493293 = LbRQdHkkDflEvyfehktco92204821;     LbRQdHkkDflEvyfehktco92204821 = LbRQdHkkDflEvyfehktco20079047;     LbRQdHkkDflEvyfehktco20079047 = LbRQdHkkDflEvyfehktco37380715;     LbRQdHkkDflEvyfehktco37380715 = LbRQdHkkDflEvyfehktco391240;     LbRQdHkkDflEvyfehktco391240 = LbRQdHkkDflEvyfehktco95784423;     LbRQdHkkDflEvyfehktco95784423 = LbRQdHkkDflEvyfehktco52813702;     LbRQdHkkDflEvyfehktco52813702 = LbRQdHkkDflEvyfehktco10315418;     LbRQdHkkDflEvyfehktco10315418 = LbRQdHkkDflEvyfehktco45799529;     LbRQdHkkDflEvyfehktco45799529 = LbRQdHkkDflEvyfehktco7684378;     LbRQdHkkDflEvyfehktco7684378 = LbRQdHkkDflEvyfehktco38313121;     LbRQdHkkDflEvyfehktco38313121 = LbRQdHkkDflEvyfehktco14254067;     LbRQdHkkDflEvyfehktco14254067 = LbRQdHkkDflEvyfehktco24418367;     LbRQdHkkDflEvyfehktco24418367 = LbRQdHkkDflEvyfehktco99087364;     LbRQdHkkDflEvyfehktco99087364 = LbRQdHkkDflEvyfehktco94646768;     LbRQdHkkDflEvyfehktco94646768 = LbRQdHkkDflEvyfehktco47198015;     LbRQdHkkDflEvyfehktco47198015 = LbRQdHkkDflEvyfehktco77264294;     LbRQdHkkDflEvyfehktco77264294 = LbRQdHkkDflEvyfehktco14698572;     LbRQdHkkDflEvyfehktco14698572 = LbRQdHkkDflEvyfehktco31543833;     LbRQdHkkDflEvyfehktco31543833 = LbRQdHkkDflEvyfehktco97402662;     LbRQdHkkDflEvyfehktco97402662 = LbRQdHkkDflEvyfehktco8224894;     LbRQdHkkDflEvyfehktco8224894 = LbRQdHkkDflEvyfehktco40321096;     LbRQdHkkDflEvyfehktco40321096 = LbRQdHkkDflEvyfehktco84289636;     LbRQdHkkDflEvyfehktco84289636 = LbRQdHkkDflEvyfehktco61914776;     LbRQdHkkDflEvyfehktco61914776 = LbRQdHkkDflEvyfehktco6040999;     LbRQdHkkDflEvyfehktco6040999 = LbRQdHkkDflEvyfehktco98947257;     LbRQdHkkDflEvyfehktco98947257 = LbRQdHkkDflEvyfehktco89056280;     LbRQdHkkDflEvyfehktco89056280 = LbRQdHkkDflEvyfehktco19804287;     LbRQdHkkDflEvyfehktco19804287 = LbRQdHkkDflEvyfehktco86770719;     LbRQdHkkDflEvyfehktco86770719 = LbRQdHkkDflEvyfehktco30564757;     LbRQdHkkDflEvyfehktco30564757 = LbRQdHkkDflEvyfehktco53456252;     LbRQdHkkDflEvyfehktco53456252 = LbRQdHkkDflEvyfehktco57036280;     LbRQdHkkDflEvyfehktco57036280 = LbRQdHkkDflEvyfehktco8578593;     LbRQdHkkDflEvyfehktco8578593 = LbRQdHkkDflEvyfehktco80001866;     LbRQdHkkDflEvyfehktco80001866 = LbRQdHkkDflEvyfehktco78265891;     LbRQdHkkDflEvyfehktco78265891 = LbRQdHkkDflEvyfehktco21374113;     LbRQdHkkDflEvyfehktco21374113 = LbRQdHkkDflEvyfehktco73781722;     LbRQdHkkDflEvyfehktco73781722 = LbRQdHkkDflEvyfehktco86611311;     LbRQdHkkDflEvyfehktco86611311 = LbRQdHkkDflEvyfehktco55103353;     LbRQdHkkDflEvyfehktco55103353 = LbRQdHkkDflEvyfehktco81283976;     LbRQdHkkDflEvyfehktco81283976 = LbRQdHkkDflEvyfehktco66199610;     LbRQdHkkDflEvyfehktco66199610 = LbRQdHkkDflEvyfehktco43363275;     LbRQdHkkDflEvyfehktco43363275 = LbRQdHkkDflEvyfehktco9270759;     LbRQdHkkDflEvyfehktco9270759 = LbRQdHkkDflEvyfehktco86904399;     LbRQdHkkDflEvyfehktco86904399 = LbRQdHkkDflEvyfehktco10678721;     LbRQdHkkDflEvyfehktco10678721 = LbRQdHkkDflEvyfehktco90534886;     LbRQdHkkDflEvyfehktco90534886 = LbRQdHkkDflEvyfehktco34068476;     LbRQdHkkDflEvyfehktco34068476 = LbRQdHkkDflEvyfehktco53810016;     LbRQdHkkDflEvyfehktco53810016 = LbRQdHkkDflEvyfehktco66937974;     LbRQdHkkDflEvyfehktco66937974 = LbRQdHkkDflEvyfehktco5194234;     LbRQdHkkDflEvyfehktco5194234 = LbRQdHkkDflEvyfehktco86482837;     LbRQdHkkDflEvyfehktco86482837 = LbRQdHkkDflEvyfehktco22878330;     LbRQdHkkDflEvyfehktco22878330 = LbRQdHkkDflEvyfehktco91654207;     LbRQdHkkDflEvyfehktco91654207 = LbRQdHkkDflEvyfehktco28230122;     LbRQdHkkDflEvyfehktco28230122 = LbRQdHkkDflEvyfehktco30565772;     LbRQdHkkDflEvyfehktco30565772 = LbRQdHkkDflEvyfehktco59172197;     LbRQdHkkDflEvyfehktco59172197 = LbRQdHkkDflEvyfehktco7915185;     LbRQdHkkDflEvyfehktco7915185 = LbRQdHkkDflEvyfehktco58164271;     LbRQdHkkDflEvyfehktco58164271 = LbRQdHkkDflEvyfehktco31339717;     LbRQdHkkDflEvyfehktco31339717 = LbRQdHkkDflEvyfehktco8225702;}
// Junk Finished
