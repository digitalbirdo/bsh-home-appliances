# BSH D-Bus Write Functionality

## Overview

The bshdbus component has been extended to support **writing** commands to the D-Bus in addition to reading. This allows you to control your BSH/Bosch/Siemens appliances programmatically via ESPHome.

## Changes Made

### 1. Core Component Updates

#### bshdbus.h
- Added `write_frame()` method to the `BSHDBus` class

#### bshdbus.cpp
- Implemented `write_frame()` method with:
  - Frame construction (length, destination, command, data)
  - CRC16 calculation and appending
  - UART transmission
  - ACK expectation handling
  - Detailed logging for debugging

#### automation.h
- Added `WriteAction` template class for ESPHome automation integration
- Supports templatable values for destination, command, and data

#### __init__.py
- Registered `bshdbus.write` action
- Added configuration schema for write operations
- Added CONF_DEST, CONF_COMMAND, CONF_DATA constants

### 2. YAML Configuration Updates

The `bsh-dbus-wm14s750.yaml` file now includes comprehensive write examples:

## Usage Examples

### Configuration Setup
First, ensure your bshdbus component has an ID defined:
```yaml
uart:
  id: dbus_uart
  rx_pin: GPIO4
  tx_pin: GPIO5
  baud_rate: 9600

bshdbus:
  id: bsh_dbus  # Define an ID for the component
  uart_id: dbus_uart
```

### Basic Button Example
```yaml
button:
  - platform: template
    name: Start/Pause Program
    icon: mdi:play-pause
    on_press:
      - bshdbus.write:
          id: bsh_dbus
          dest: 0x15
          command: 0x1100
          data: [0x01]
```

### Switch Example (Toggle Feature)
```yaml
switch:
  - platform: template
    name: Water Plus
    icon: mdi:water-plus
    optimistic: true
    turn_on_action:
      - bshdbus.write:
          id: bsh_dbus
          dest: 0x14
          command: 0x1007
          data: [0x02]  # Enable Water Plus (bit 1)
    turn_off_action:
      - bshdbus.write:
          id: bsh_dbus
          dest: 0x14
          command: 0x1007
          data: [0x00]  # Disable Water Plus
```

### Dynamic Data with Lambda
```yaml
number:
  - platform: template
    name: Custom Temperature
    min_value: 0
    max_value: 7
    step: 1
    set_action:
      - bshdbus.write:
          id: bsh_dbus
          dest: 0x14
          command: 0x1004
          data: !lambda |-
            return {(uint8_t)x};
```

## Action Parameters

### `bshdbus.write`

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `id` | ID | Yes | ID of the bshdbus component (e.g., `bsh_dbus`) |
| `dest` | uint8_t | Yes | Destination address on the D-Bus (e.g., 0x14, 0x15, 0x26) |
| `command` | uint16_t | Yes | Command code (e.g., 0x1100, 0x1004) |
| `data` | List[uint8_t] | Yes | Data bytes to send (can be empty list `[]`) |

All parameters support templating with lambdas for dynamic values.

## Frame Format

The component automatically constructs D-Bus frames in this format:
```
[LL] [DD] [CC CC] [data...] [CRC CRC]
```

Where:
- `LL` = Frame length (1 byte)
- `DD` = Destination address (1 byte)
- `CC CC` = Command code (2 bytes, big-endian)
- `data...` = Data payload (variable length)
- `CRC CRC` = CRC16 checksum (2 bytes, big-endian)

## Known Command Examples (WM14S750)

| Destination | Command | Description | Data Example |
|-------------|---------|-------------|--------------|
| 0x14 | 0x1004 | Set temperature | [0x02] = 40°C |
| 0x14 | 0x1006 | Set spin speed | [0x78] = 1200 RPM |
| 0x14 | 0x1007 | Set features | [0x02] = Water Plus on |
| 0x15 | 0x1100 | Start/Pause | [0x01] = Toggle |

**Note:** These are examples based on observed behavior. Actual command behavior may vary by appliance model. **Use at your own risk!**

## Logging

The component provides detailed logging:
- `ESP_LOGD`: Logs each frame being sent with destination, command, and data
- `ESP_LOGV`: Logs complete frame including CRC for debugging
- ACK reception and timeout messages

## Safety Considerations

⚠️ **Important Warnings:**

1. **Test carefully**: Start with non-critical operations
2. **Monitor responses**: Watch for ACK messages and appliance behavior
3. **Invalid commands**: May be ignored or cause unexpected behavior
4. **Warranty**: Modifications may void your appliance warranty
5. **Electrical safety**: Ensure proper isolation and protection

## Debugging Tips

1. Enable verbose logging in ESPHome:
   ```yaml
   logger:
     level: VERBOSE
     logs:
       bshdbus: VERBOSE
   ```

2. Monitor the D-Bus traffic to understand timing and responses

3. Use the activity LED to visualize bus activity

4. Check for ACK messages after each write

## Future Enhancements

Possible improvements:
- Detect free timeslot on the DBUS
- Add response parsing after write operations
- Implement write confirmation callbacks
- Add retry logic for failed writes
- Create high-level helper functions for common operations

## Credits

- Original component: @hn (Hajo Noerenberg)
- Write functionality extension: Community contribution
- Based on VBus component by @ssieb

## License

GNU General Public License v3.0

See the original component files for full license information.
