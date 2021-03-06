#include "QUIX.h"


SpawnerObject::SpawnerObject( char* name, char* path, unsigned long type ) : SceneObject( type )
{
		

		Script *script = NULL;

		try
		{
			// Load the spawner's script.
			script = new Script( name, path );
		}
		catch( std::bad_alloc& )
		{
			SAFE_DELETE( script );
			throw std::bad_alloc();
		}


	// Set spawner objects as ghosts.
	SetGhost( true );

	// Get the spawner's frequency.
	m_frequency = *script->GetFloatData( "frequency" );

	// Clear the spawn timer.
	m_spawnTimer = 0.0f;

	// Load the sound to play when the spawner's object is collected.
	if( script->GetStringData( "sound" ) != NULL )
	{
		m_sound = new Sound( script->GetStringData( "sound" ) );
		//m_sound->SetMinAttDist( 1.0f / g_engine->GetScale() );
		m_audioPath = new AudioPath3D;
	}
	else
	{
		m_sound = NULL;
		m_audioPath = NULL;
	}

	// Load the script for the spawner's object.
	m_objectScript = g_engine->GetScriptManager()->Add( script->GetStringData( "object" ), script->GetStringData( "object_path" ) );

	// Get the name of the spawner's object.
	m_name = new char[strlen( m_objectScript->GetStringData( "name" ) ) + 1];
	strcpy( m_name, m_objectScript->GetStringData( "name" ) );

	// Set the spawner's mesh to use the object's mesh.
	SetMesh( m_objectScript->GetStringData( "mesh" ), m_objectScript->GetStringData( "mesh_path" ) );

	// Set the object to spin slowly.
	SetSpin( 0.0f, 1.0f, 0.0f );

	// Get the spawner's radius. A radius of 0.0 indicates that it must be
	// taken from the object to be spawned.
	if( *script->GetFloatData( "radius" ) != 0.0f )
		SetBoundingSphere( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), *script->GetFloatData( "radius" ) );
	else if( GetMesh() != NULL )
		SetEllipsoidRadius( *m_objectScript->GetVectorData( "ellipse_radius" ) );

	// Destroy the spawner's script.
	SAFE_DELETE( script );
}

SpawnerObject::~SpawnerObject()
{
	// Destroy the string buffer containing the name of the spawner's object.
	SAFE_DELETE_ARRAY( m_name );

	// Destroy the collect sound and its audio path.
	SAFE_DELETE( m_sound );
	SAFE_DELETE( m_audioPath );
	
	// Destroy the spawner's object script.
	g_engine->GetScriptManager()->Remove( &m_objectScript );
}

void SpawnerObject::Update( float elapsed, bool addVelocity )
{
	// Allow the base scene object to update.
	SceneObject::Update( elapsed, addVelocity );

	// Check if the spawner is not visible
	if( GetVisible() == false )
	{
		// The spawner will become visible again after the set amount of
		// time (frequency) has passed.
		m_spawnTimer += elapsed;
		if( m_spawnTimer >= m_frequency )
		{
			SetVisible( true );
			SetIgnoreCollisions( false );
			m_spawnTimer = 0.0f;
		}
	}

	// Update the collect sound's audio path.
	if( m_sound != NULL )
	{
		Vector3 pos = Vector3( GetTranslation().x, GetTranslation().y, GetTranslation().z );
		Vector3 vel = Vector3( GetVelocity().x, GetVelocity().y, GetVelocity().z );
		m_audioPath->SetPosition( pos );
		m_audioPath->SetVelocity( vel );
	}
}

void SpawnerObject::CollisionOccurred( SceneObject *object, unsigned long collisionStamp )
{
	// Allow the base scene object to register the collision.
	SceneObject::CollisionOccurred( object, collisionStamp );

	
	// Return if the object is not allowed to pick up this spawner
	if( object->ValidateSpawnerCollision(this) == false )
		return;


	// Make the spawner invisible. This will prevent other objects from
	// colliding with it until it respawns (i.e. becomes visible again).
	SetVisible( false );
	SetIgnoreCollisions( true );

	// Play the collected sound.
	if( m_audioPath != NULL && m_sound != NULL )
	{
		m_audioPath->Play( m_sound->GetSound() );
	}
}

Script *SpawnerObject::GetObjectScript()
{
	return m_objectScript;
}