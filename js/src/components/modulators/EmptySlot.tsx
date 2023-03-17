import {
  Card,
  Center,
  Flex,
  List,
  Menu,
  Modal,
  NavLink,
  Paper,
  Stack,
  Text,
  ThemeIcon,
} from "@mantine/core";
import { range, useDisclosure } from "@mantine/hooks";
import { observer } from "mobx-react";
import { FC, ReactNode, useContext } from "react";
import { TbPlus, TbArrowWaveRightDown } from "react-icons/tb";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { ParameterDial } from "../ParameterDial";

interface Props {}

export const EmptySlot: FC<Props> = observer(() => {
  const parameters = useContext(ParametersContext)!;

  const [opened, { open, close }] = useDisclosure(false);

  return (
    <>
      <Card
        shadow="lg"
        p="md"
        radius="md"
        mih={200}
        style={{
          borderStyle: "dashed",
          display: "flex",
          alignItems: "stretch",
        }}
        onClick={open}
        withBorder
      >
        <Flex align="center" justify="center" style={{ flex: 1 }}>
          <TbPlus size={32} />
        </Flex>
      </Card>
      <Modal
        opened={opened}
        onClose={close}
        withCloseButton={false}
        centered
        size="auto"
      >
        {range(0, 4).map(
          (index) =>
            !(parameters as any)[`lfo${index + 1}Enabled`].value && (
              <NavLink
                label={`LFO ${index + 1}`}
                icon={<TbArrowWaveRightDown size="1rem" />}
                onClick={() => {
                  close();
                  setTimeout(
                    () =>
                      ((parameters as any)[`lfo${index + 1}Enabled`].value =
                        true),
                    200
                  );
                }}
              />
            )
        )}
      </Modal>
    </>
  );
});
