import os
import json

# Define the cache directory and file path
CACHE_DIR = os.path.join(os.path.expanduser("~"), ".clboss")
CACHE_FILE = os.path.join(CACHE_DIR, "alias_cache.json")

def load_cache():
    if os.path.exists(CACHE_FILE):
        with open(CACHE_FILE, 'r') as f:
            return json.load(f)
    return {}

def save_cache(cache):
    # Ensure the cache directory exists
    if not os.path.exists(CACHE_DIR):
        os.makedirs(CACHE_DIR)

    with open(CACHE_FILE, 'w') as f:
        json.dump(cache, f)

def lookup_alias(run_lightning_cli_command, network_option, peer_id):
    # Load the cache
    cache = load_cache()

    # Check if the alias is already cached
    if peer_id in cache:
        return cache[peer_id]

    # Perform the lookup
    alias = peer_id  # Default to peer_id if alias not found
    listnodes_data = run_lightning_cli_command(network_option, 'listnodes', peer_id)
    if listnodes_data:
        nodes = listnodes_data.get("nodes", [])
        for node in nodes:
            alias = node.get("alias", peer_id)  # Fallback to peer_id if alias not found

    # Cache the result
    cache[peer_id] = alias
    save_cache(cache)

    return alias
