extends GutTest

# Configuration and setup tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Configuration Tests
# ============================================================================

func test_get_version():
	var version = converter.get_version()
	assert_not_null(version, "Version should not be null")
	assert_typeof(version, TYPE_STRING, "Version should be a string")
	assert_gt(version.length(), 0, "Version string should not be empty")

func test_get_default_config():
	var config = converter.get_config()
	assert_not_null(config, "Config should not be null")
	assert_typeof(config, TYPE_DICTIONARY, "Config should be a dictionary")

	# Check for expected default config keys
	assert_true(config.has("inferTypes"), "Config should have inferTypes")
	assert_true(config.has("strictTypes"), "Config should have strictTypes")

func test_set_config():
	var new_config = {
		"inferTypes": false,
		"strictTypes": true,
		"verbose": true
	}

	converter.set_config(new_config)
	var result_config = converter.get_config()

	assert_eq(result_config["inferTypes"], false, "inferTypes should be false")
	assert_eq(result_config["strictTypes"], true, "strictTypes should be true")
	assert_eq(result_config["verbose"], true, "verbose should be true")
